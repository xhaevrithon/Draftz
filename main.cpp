//============================================================
// Draftz - Simple Text Editor using FLTK
//
// Features:
//  - Write text
//  - New
//  - Open
//  - Save
//  - Undo
//  - Redo
//
// Everything is inside one file so it's easy to learn.
//
// Compile:
//  Using CMake:
//   mkdir build && cd build && cmake .. && make
//
//  Using Make:
//   make
//
//  Direct compilation:
//   g++ main.cpp -std=c++17 -lfltk -lfltk_images -o Draftz

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <fstream>
#include <sstream>
#include <string>

#include "header/features.h" // our custom Undo/Redo manager

// -----------------------------------------------------------------------
// EditorWidget
// -----------------------------------------------------------------------
// WHY THIS CLASS EXISTS:
// Fl_Text_Editor comes with its OWN built-in keyboard shortcut wired
// directly into the widget: Ctrl+Z is hard-coded to FLTK's internal
// kf_undo(). That function is a primitive, single-slot undo that has
// nothing to do with our UndoRedoManager - and it behaves like a
// TOGGLE (press it twice and you flip back and forth between just two
// states forever).
//
// Because the text area has keyboard focus while you're typing, FLTK
// hands Ctrl+Z to THIS WIDGET first - before our menu's shortcut ever
// gets a chance to run. So pressing Ctrl+Z was silently triggering
// FLTK's built-in toggle-undo instead of ours, and that also polluted
// our own undo history in the background. That's exactly why undo felt
// broken and redo did nothing.
//
// The fix: subclass the editor and grab Ctrl+Z / Ctrl+Y ourselves in
// handle(), BEFORE the base class (and its built-in shortcut table)
// ever sees the keypress. That way every undo/redo, whether triggered
// by keyboard or by clicking the menu, goes through our ONE correct
// UndoRedoManager.
// -----------------------------------------------------------------------
class EditorWidget : public Fl_Text_Editor
{
public:
    // Simple callbacks the owning TextEditor sets, so this widget can
    // ask it to perform undo/redo without needing to know its internals.
    void (*onUndo)(void*) = nullptr;
    void (*onRedo)(void*) = nullptr;
    void* owner = nullptr;

    EditorWidget(int X, int Y, int W, int H) : Fl_Text_Editor(X, Y, W, H) {}

    int handle(int event) override
    {
        if (event == FL_KEYDOWN || event == FL_SHORTCUT)
        {
            bool ctrl = Fl::event_ctrl() != 0;
            bool shift = Fl::event_shift() != 0;
            int key = Fl::event_key();

            if (ctrl && !shift && key == 'z')
            {
                if (onUndo)
                    onUndo(owner);
                return 1; // "handled" - stops FLTK's built-in undo from running
            }

            if (ctrl && key == 'y')
            {
                if (onRedo)
                    onRedo(owner);
                return 1;
            }
        }

        // Anything else (typing, arrows, backspace, copy/paste, etc.)
        // goes through the normal Fl_Text_Editor behaviour as usual.
        return Fl_Text_Editor::handle(event);
    }
};

class TextEditor
{
private:
    Fl_Window window;
    Fl_Menu_Bar menu;
    Fl_Text_Buffer buffer;
    EditorWidget editor;

    UndoRedoManager undoRedo;

    // Remembers where the current document was last saved/opened from.
    // Empty means "this document has never been saved yet".
    // This is what makes Save behave differently from Save As:
    //   - Save:    if we already know a path, write to it immediately.
    //              if not (brand new file), fall back to asking, just
    //              like Save As.
    //   - Save As: always asks for a new name/location, no matter what.
    std::string currentFilePath;

    // Snapshot of what the text looked like right before the most
    // recent change. We need this because Fl_Text_Buffer only tells us
    // about a change AFTER it already happened, not before.
    std::string textBeforeChange;

public:
    TextEditor()
        : window(900, 600, "Draftz Text Editor"),
          menu(0, 0, 900, 30),
          editor(0, 30, 900, 570)
    {
        editor.buffer(&buffer);

        // Hook the editor widget up to our own undo/redo, and give it
        // a pointer back to "this" so it can call TextEditor::undo()/redo().
        editor.owner = this;
        editor.onUndo = [](void* self) { static_cast<TextEditor*>(self)->undo(); };
        editor.onRedo = [](void* self) { static_cast<TextEditor*>(self)->redo(); };

        // Whenever the buffer's text changes (typing, deleting, pasting,
        // or us calling buffer.text(...) to load a file), FLTK calls
        // bufferChangedCallback(). That's how we detect changes.
        buffer.add_modify_callback(bufferChangedCallback, this);

        menu.add("&File/&New", FL_CTRL + 'n', menuCallback, this);
        menu.add("&File/&Open", FL_CTRL + 'o', menuCallback, this);
        menu.add("&File/&Save", FL_CTRL + 's', menuCallback, this);
        menu.add("&File/Save &As...", FL_CTRL + FL_SHIFT + 's', menuCallback, this);

        // NOTE: no FL_CTRL+'z' / FL_CTRL+'y' shortcut is registered here
        // on purpose. Keyboard undo/redo is handled by EditorWidget
        // above (see the big comment on that class for why). These menu
        // entries can still be triggered by clicking them with the mouse.
        menu.add("&Edit/&Undo", 0, menuCallback, this);
        menu.add("&Edit/&Redo", 0, menuCallback, this);

        window.end();
    }

    void run()
    {
        window.show();
    }

private:
    // ------------------------------------------------------------
    // Menu click handler - figures out which menu item was clicked
    // and calls the matching function.
    // ------------------------------------------------------------
    static void menuCallback(Fl_Widget* widget, void* data)
    {
        TextEditor* app = static_cast<TextEditor*>(data);

        const Fl_Menu_Item* item = app->menu.mvalue();
        if (item == nullptr)
            return;

        std::string choice = item->label();

        if (choice == "&New")
            app->newFile();
        else if (choice == "&Open")
            app->openFile();
        else if (choice == "&Save")
            app->saveFile();
        else if (choice == "Save &As...")
            app->saveFileAs();
        else if (choice == "&Undo")
            app->undo();
        else if (choice == "&Redo")
            app->redo();
    }

    // ------------------------------------------------------------
    // Called automatically by FLTK every time the buffer's text
    // changes for ANY reason (typing, deleting, or us loading text).
    // ------------------------------------------------------------
    static void bufferChangedCallback(int pos, int nInserted, int nDeleted,
                                       int nRestyled, const char* deletedText,
                                       void* data)
    {
        // Ignore "changes" that are just cursor/style updates with no
        // actual text inserted or deleted.
        if (nInserted == 0 && nDeleted == 0)
            return;

        TextEditor* app = static_cast<TextEditor*>(data);
        app->onTextChanged();
    }

    void onTextChanged()
    {
        // Save what the text was BEFORE this change into the undo
        // history (the manager itself decides whether to actually
        // record it, e.g. it skips recording changes caused by
        // undo()/redo() themselves).
        undoRedo.recordChange(textBeforeChange);

        // Update our snapshot so next time we know the "before" state.
        textBeforeChange = getBufferText();
    }

    // Helper: safely reads the buffer's text into a std::string.
    // (Fl_Text_Buffer::text() returns a malloc'd char* that we must free.)
    std::string getBufferText()
    {
        char* raw = buffer.text();
        std::string text = raw ? raw : "";
        if (raw)
            free(raw);
        return text;
    }

    void newFile()
    {
        buffer.text("");
        textBeforeChange = "";
        undoRedo.reset(); // fresh document = fresh history
        currentFilePath.clear(); // brand new file has no location yet
    }

    void openFile()
    {
        Fl_Native_File_Chooser chooser;
        chooser.title("open");
        chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
        if (chooser.show() != 0)
            return;

        std::ifstream file(chooser.filename());
        if (!file)
            return;

        std::stringstream stream;
        stream << file.rdbuf();

        buffer.text(stream.str().c_str());
        textBeforeChange = getBufferText();
        undoRedo.reset(); // opening a different file = fresh history
        currentFilePath = chooser.filename(); // remember where this came from
    }

    // ------------------------------------------------------------
    // Save: if this document already has a known location (either it
    // was opened from disk, or it's been saved once before this
    // session), write straight to that path with no dialog.
    // Otherwise (a brand new, never-saved file) it falls back to
    // asking for a location, exactly like Save As.
    // ------------------------------------------------------------
    void saveFile()
    {
        if (currentFilePath.empty())
        {
            saveFileAs();
            return;
        }

        writeToFile(currentFilePath);
    }

    // Save As: always opens the "save" dialog and asks for a
    // name/location, even if the document was already saved before.
    void saveFileAs()
    {
        Fl_Native_File_Chooser chooser;
        chooser.title("save as");
        chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
        if (chooser.show() != 0)
            return;

        currentFilePath = chooser.filename();
        writeToFile(currentFilePath);
    }

    // Shared helper that actually writes the buffer's text to disk.
    void writeToFile(const std::string& path)
    {
        std::ofstream file(path);
        if (!file)
            return;

        char* text = buffer.text();
        file << text;
        free(text);
    }

    // ------------------------------------------------------------
    // Undo / Redo - both go through our own UndoRedoManager instead
    // of FLTK's limited built-in kf_undo(), so both work reliably
    // and support multiple steps.
    // ------------------------------------------------------------
    void undo()
    {
        if (!undoRedo.canUndo())
            return;

        std::string restoredText = undoRedo.undo(getBufferText());
        buffer.text(restoredText.c_str());
        textBeforeChange = restoredText;
    }

    void redo()
    {
        if (!undoRedo.canRedo())
            return;

        std::string restoredText = undoRedo.redo(getBufferText());
        buffer.text(restoredText.c_str());
        textBeforeChange = restoredText;
    }
};

int main()
{
    TextEditor app;
    app.run();
    return Fl::run();
}

//
//============================================================
