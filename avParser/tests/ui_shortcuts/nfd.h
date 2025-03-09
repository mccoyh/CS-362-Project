#ifndef NATIVEFILEDIALOG_H
#define NATIVEFILEDIALOG_H

#ifdef __cplusplus
extern "C" {
#endif

// Result of the file dialog operations
typedef enum {
    NFD_ERROR,       // Error occurred
    NFD_OKAY,        // User selected a file
    NFD_CANCEL       // User canceled the dialog
} nfdresult_t;

// Callback result from the Open/Save dialog, used for file path
typedef const char* nfdchar_t;

// Filter for file types
typedef struct {
    const char *name;   // A readable description of the file type
    const char *spec;   // The filter, usually extension(s), comma-separated
} nfdfilteritem_t;

// Opens a file dialog and lets the user choose a file
nfdresult_t NFD_OpenDialog(nfdchar_t **outPath, nfdfilteritem_t *filters, unsigned int numFilters, const char *defaultPath);

// Fetches the last error message.
const char* NFD_GetError();

// Frees the path returned by NFD_OpenDialog
void NFD_FreePath(nfdchar_t *path);

#ifdef __cplusplus
}
#endif

#endif // NATIVEFILEDIALOG_H