import ctypes

libc = ctypes.CDLL("projects\\c_dll\\bin\\c_dll.dll")

# Input file for encryption/decryption should be in the main working folder of the project!
# Error codes
# err_code == 0 - everything is OK
# err_code == 1 - input file doesn't exist
# err_code == 2 - password is shorter than 8 characters

# ======== encrypt file =========
encrypt_file = libc.encrypt_file
encrypt_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
# Arguments for the function
input_file_ = b"input_file.txt"
result_file_ = b"result_of_encryption.txt"
password_ = b"password"
# Pointers for dll
input_file_ptr = ctypes.c_char_p(input_file_)
result_file_ptr = ctypes.c_char_p(result_file_)
password_ptr = ctypes.c_char_p(password_)
# Calling fucntion
err_code = encrypt_file(input_file_ptr, result_file_ptr, password_ptr)
if err_code == 1:
    print("Can't open file ", input_file_)
elif err_code == 2:
    print("Password is too short, it should be at least 8 characters")
else:
    print(f"File {input_file_} encrypted successfully, result in {result_file_}")

# ======== decrypt file =========
decrypt_file = libc.decrypt_file
decrypt_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
# Arguments for the function
input_file_ = b"result_of_encryption.txt"
result_file_ = b"result_of_decryption.txt"
password_ = b"password"
# Pointers for dll
input_file_ptr = ctypes.c_char_p(input_file_)
result_file_ptr = ctypes.c_char_p(result_file_)
password_ptr = ctypes.c_char_p(password_)
# Calling fucntion
err_code = decrypt_file(input_file_ptr, result_file_ptr, password_ptr)
if err_code == 1:
    print("Can't open file", input_file_)
elif err_code == 2:
    print("Password is too short, it should be at least 8 characters")
else:
    print(f"File {input_file_} decrypted successfully, result in {result_file_}")