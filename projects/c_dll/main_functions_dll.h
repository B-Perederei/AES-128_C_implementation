#ifndef MAIN_FUNCTIONS_DLL_H
#define MAIN_FUNCTIONS_DLL_H

#define DllExport __declspec(dllexport)

DllExport int encrypt_file(char *input_file, char *result_file, char *password);
DllExport int decrypt_file(char *input_file, char *result_file, char *password);

#endif