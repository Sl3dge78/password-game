bool IsLeadingByte(const char c) {
    bool first_bit_set = (c & 0x80) != 0;
    bool second_bit_set = (c & 0X40) != 0;
    return !first_bit_set || second_bit_set;
}

bool IsUpper(const char c) {
    return (c >= 'A' && c <= 'Z');
}

bool IsLower(const char c) {
    return (c >= 'a' && c <= 'z');
}