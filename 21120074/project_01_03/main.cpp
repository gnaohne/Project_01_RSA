#include<iostream>
#include<bitset>
#include<fstream>
#include<string>
#include<chrono>
#include<algorithm>

#define MAX_SIZE 1025

using namespace std;

//string input_file_name = "test_09.inp";
//string output_file_name = "test_09.out";

bitset<MAX_SIZE> hex_str_to_bin(const string& hex);
bitset<4> hex_char_to_bin(char c);
bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary);

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: $ ./main test_XX.inp test_XX.out" << endl;
        return 1;
    }

    ifstream input_file(argv[1]);

    //ifstream input_file(input_file_name);

    string N_str, k_str, x_str;

    if (!input_file.is_open())
    {
        cout << "Can't open file " << argv[1] << endl;
        return 1;
    }

    getline(input_file, N_str, '\n');
    getline(input_file, k_str, '\n');
    getline(input_file, x_str, '\n');
    input_file.close();

    reverse(N_str.begin(), N_str.end());
    reverse(k_str.begin(), k_str.end());
    reverse(x_str.begin(), x_str.end());


    bitset<MAX_SIZE> N = hex_str_to_bin(N_str);
    bitset<MAX_SIZE> k = hex_str_to_bin(k_str);
    bitset<MAX_SIZE> x = hex_str_to_bin(x_str);

    ofstream output_file(argv[2]);
    //ofstream output_file(output_file_name);

    if (output_file.is_open())
    {
        bitset<MAX_SIZE> y = power_mod(x, k, N);
        string hex_y = binary_to_hex_little_endian(y);
        //cout << hex_y << endl;
        output_file << hex_y;
        output_file.close();
    }
    else {
        cout << "Can't open file " << argv[2] << endl;
        return 1;
    }

    return 0;
}

bitset<4> hex_char_to_bin(char c)
{
    switch (toupper(c))
    {
    case '0': return bitset<4>("0000");
    case '1': return bitset<4>("0001");
    case '2': return bitset<4>("0010");
    case '3': return bitset<4>("0011");
    case '4': return bitset<4>("0100");
    case '5': return bitset<4>("0101");
    case '6': return bitset<4>("0110");
    case '7': return bitset<4>("0111");
    case '8': return bitset<4>("1000");
    case '9': return bitset<4>("1001");
    case 'A': return bitset<4>("1010");
    case 'B': return bitset<4>("1011");
    case 'C': return bitset<4>("1100");
    case 'D': return bitset<4>("1101");
    case 'E': return bitset<4>("1110");
    }
    return bitset<4>("1111");
}

bitset<MAX_SIZE> hex_str_to_bin(const string& hex)
{
    bitset<MAX_SIZE> bin;
    int length = hex.length() * 4;

    for (int i = 0; i < length; ++i)
    {
        bin <<= 1;
        char hex_char = hex[i / 4];
        int hex_value = hex_char_to_bin(hex_char).to_ulong();
        int shift = 3 - (i % 4);
        int bit = (hex_value >> shift) & 1;
        bin |= bit;
    }

    return bin;
}

bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n)
{
    auto start_1 = std::chrono::high_resolution_clock::now();

    bitset<MAX_SIZE> result(1);

    bool start = false;

    for (int i = a.size() - 1; i >= 0; --i)
    {
        if (a[i] == 1)start = true;

        if (start)
        {
            // y = y^2 (mod n)
            result = mul_mod(result, result, n);

            if (a[i] == 1)
            {
                result = mul_mod(result, x, n); // y = y*x (mod n)
            }
        }
        else continue;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start_1).count();
        if (duration > 60)
        {
            cerr << "Time out!";
            exit(1);
        }
    }

    return result;
}

bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    bitset<MAX_SIZE> result = a;
    result ^= b;
    bitset<MAX_SIZE> carry = (a & b) << 1;
    while (carry != 0)
    {
        bitset<MAX_SIZE> temp_result = result;
        result ^= carry;
        carry = (temp_result & carry) << 1;
    }
    return result;
}

int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    for (int i = a.size() - 1; i >= 0; --i)
    {
        if (a[i] < b[i])
        {
            return -1; // a < b
        }
        else if (a[i] > b[i])
        {
            return 1; // a > b
        }
    }
    return 0; // a = b
}


bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{

    bitset<MAX_SIZE> result = a;
    result ^= b;
    bitset<MAX_SIZE> borrow = ((~a) & b) << 1;
    while (borrow != 0)
    {
        bitset<MAX_SIZE> temp_result = result;
        result ^= borrow;
        borrow = ((~temp_result) & borrow) << 1;
    }
    return result;
}

bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n)
{

    bool compare = compare_binary(x, n);
    if (compare == 0) return bitset<MAX_SIZE>(0);
    if (compare == -1) return x;

    bitset<MAX_SIZE> adjusted_x = x;
    bitset<MAX_SIZE> adjusted_n = n;

    while (compare_binary(adjusted_x, adjusted_n) == 1 || compare_binary(adjusted_x, adjusted_n) == 0)
    {
        adjusted_x = subtract_binary(adjusted_x, adjusted_n);
    }

    return adjusted_x;
}

bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n)
{
    bitset<MAX_SIZE> temp_x = modulo(x, n);
    bitset<MAX_SIZE> result(0);

    int count = 0;
    for (int i = MAX_SIZE - 1; i >= 0; --i)
    {
        if (y[i] == 1) break;
        else
        {
            count++;
        }
    }

    if (y[0] == 1)
    {
        result = temp_x;
    }

    for (int i = 1; i < MAX_SIZE - count; ++i)
    {
        temp_x <<= 1; // temp_x = temp_x * 2
        temp_x = modulo(temp_x, n);

        if (y[i] == 1) {
            result = add_binary(result, temp_x);
            result = modulo(result, n);
        }
    }
    return result;
}

string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary)
{
    string bin;
    for (int i = 0; i < MAX_SIZE; ++i)
        bin += (binary[i] + 48);

    int last_one_index = -1;
    for (int i = bin.size() - 1; i >= 0; --i)
    {
        if (bin[i] == '1')
        {
            last_one_index = i;
            break;
        }
    }

    if (last_one_index == -1) return "0";

    string result = bin.substr(0, last_one_index + 1);

    while (result.size() % 4 != 0)
    {
        result = result + "0";
    }

    reverse(result.begin(), result.end());

    string rest, tmp;

    for (int i = 0; i < result.length(); i += 4)
    {
        tmp = result.substr(i, 4);
        if (!tmp.compare("0000"))
        {
            rest = "0" + rest;
        }
        else if (!tmp.compare("0001"))
        {
            rest = "1" + rest;
        }
        else if (!tmp.compare("0010"))
        {
            rest = "2" + rest;
        }
        else if (!tmp.compare("0011"))
        {
            rest = "3" + rest;
        }
        else if (!tmp.compare("0100"))
        {
            rest = "4" + rest;
        }
        else if (!tmp.compare("0101"))
        {
            rest = "5" + rest;
        }
        else if (!tmp.compare("0110"))
        {
            rest = "6" + rest;
        }
        else if (!tmp.compare("0111"))
        {
            rest = "7" + rest;
        }
        else if (!tmp.compare("1000"))
        {
            rest = "8" + rest;
        }
        else if (!tmp.compare("1001"))
        {
            rest = "9" + rest;
        }
        else if (!tmp.compare("1010"))
        {
            rest = "A" + rest;
        }
        else if (!tmp.compare("1011"))
        {
            rest = "B" + rest;
        }
        else if (!tmp.compare("1100"))
        {
            rest = "C" + rest;
        }
        else if (!tmp.compare("1101"))
        {
            rest = "D" + rest;
        }
        else if (!tmp.compare("1110"))
        {
            rest = "E" + rest;
        }
        else if (!tmp.compare("1111"))
        {
            rest = "F" + rest;
        }
        else
        {
            continue;
        }
    }
    return rest;
}