#ifndef FEATURES_H
#define FEATURES_H

#include <string>
#include <vector>

// -----------------------------------------------------------------------
// UndoRedoManager
// -----------------------------------------------------------------------
// FLTK's Fl_Text_Editor has a built-in undo (kf_undo), but it only
// remembers ONE step back and has NO redo feature at all.
//
// To get real multi-step Undo + Redo, we keep our own history of the
// text ourselves. Every time the text changes, we save what it looked
// like BEFORE the change into "undoStack". When the user hits Undo,
// we pop that old text back out and put the "undone" text into
// "redoStack" so Redo can bring it back later.
//
// This is a simple "snapshot" based system - easy to understand,
// though not the most memory-efficient for huge files. That's fine
// for a lightweight text editor like this one.
// -----------------------------------------------------------------------
class UndoRedoManager
{
private:
    std::vector<std::string> undoStack; // older versions of the text
    std::vector<std::string> redoStack; // versions we "undid" (so we can redo them)

    // When we restore text because of an Undo/Redo action, that restore
    // itself would normally get detected as "a new change" and saved
    // again. This flag tells the editor to skip recording that one
    // specific change.
    bool suppressNextRecord = false;

public:
    // Call this whenever the text is about to change because the USER
    // typed/deleted something (not because of Undo/Redo itself).
    // "textBeforeChange" is what the buffer looked like just before
    // this edit happened.
    void recordChange(const std::string& textBeforeChange)
    {
        if (suppressNextRecord)
        {
            // This change was caused by our own undo()/redo() call,
            // so we don't want to record it as a brand-new edit.
            suppressNextRecord = false;
            return;
        }

        undoStack.push_back(textBeforeChange);

        // Once the user makes a fresh edit, the old "redo" history
        // no longer makes sense, so we clear it.
        redoStack.clear();
    }

    bool canUndo() const { return !undoStack.empty(); }
    bool canRedo() const { return !redoStack.empty(); }

    // Moves one step back in history.
    // "currentText" is what the buffer looks like right now (so we can
    // save it into redoStack before overwriting it).
    // Returns the text that should now be put into the buffer.
    std::string undo(const std::string& currentText)
    {
        std::string previousText = undoStack.back();
        undoStack.pop_back();

        redoStack.push_back(currentText);

        suppressNextRecord = true; // ignore the change this undo triggers
        return previousText;
    }

    // Moves one step forward in history (re-applies a change that was
    // just undone).
    std::string redo(const std::string& currentText)
    {
        std::string nextText = redoStack.back();
        redoStack.pop_back();

        undoStack.push_back(currentText);

        suppressNextRecord = true; // ignore the change this redo triggers
        return nextText;
    }

    // Call this when starting a new file / opening a file, since old
    // undo/redo history shouldn't apply to a completely different document.
    void reset()
    {
        undoStack.clear();
        redoStack.clear();
        suppressNextRecord = false;
    }
};

#endif // FEATURES_H
