#include<iostream>
#include<bitset>
#include<fstream>
#include<string>
#include<chrono>
#include<algorithm>

#define MAX_SIZE 1025 // 1 bit sign 
#define SIZE_DEFAUT 512

using namespace std;

//string input_file_name = "test_11.inp";
//string output_file_name = "test_11.out";

bitset<4> hex_char_to_bin(char c);
bitset<MAX_SIZE> hex_str_to_bin(const string& hex);
bitset<MAX_SIZE> bezout_binary(bitset<MAX_SIZE>& e, const bitset<MAX_SIZE>& p, const bitset<MAX_SIZE>& q, bitset<MAX_SIZE>& d);
bitset<MAX_SIZE> mul_binary(const bitset<MAX_SIZE>& p, const bitset<MAX_SIZE>& q); // cal phi_n = (p-1)(q-1)
bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
string binary_to_hex_little_endian(const bitset<MAX_SIZE>& binary);

int main(int argc, char* argv[])
{    
    if (argc != 3)
    {
        cerr << "Usage: $ ./main test_XX.inp test_XX.out" << endl;
        return 1;
    }
    ifstream input_file(argv[1]);

    //ifstream input_file(input_file_name);

    string p_str,q_str,e_str;

    if (!input_file.is_open())
    {
        cout << "Can't open file " << argv[1] << endl;
        return 1;
    }
 
    getline(input_file, p_str, '\n');
    getline(input_file, q_str, '\n');
    getline(input_file, e_str, '\n');
    input_file.close();

    reverse(p_str.begin(), p_str.end());
    reverse(q_str.begin(), q_str.end());
    reverse(e_str.begin(), e_str.end());

    
    bitset<MAX_SIZE> p = hex_str_to_bin(p_str);
    bitset<MAX_SIZE> q = hex_str_to_bin(q_str);
    bitset<MAX_SIZE> e = hex_str_to_bin(e_str);
    bitset<MAX_SIZE> d(0);

    bitset<MAX_SIZE> gcd = bezout_binary(e, p, q, d);

    //cout << "d_binary: " << d << endl;

    ofstream output_file(argv[2]);
    //ofstream output_file(output_file_name);

    if (!output_file.is_open())
    {
        cout << "Can't open file " << argv[2] << endl;
        return 1;
    }
   
    if (compare_binary(gcd, bitset<MAX_SIZE>(1)) == 0)
    {
        string hex = binary_to_hex_little_endian(d);
        //cout << "hex: " << hex << endl;
        output_file << hex;
    }
    else
    {
        //cout << -1 << endl;
        output_file << -1;
    }
    output_file.close();
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

bitset<MAX_SIZE> mul_binary(const bitset<MAX_SIZE>& p, const bitset<MAX_SIZE>& q)
{
    bitset<MAX_SIZE> result(0);

    for (int i = 0; i < SIZE_DEFAUT; ++i) // because p and q maximum 512 bit
    {
        if (q[i])
        {
            bitset<MAX_SIZE> shifted_p = p;
            shifted_p <<= i;    
            result = add_binary(result, shifted_p);
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

bitset<MAX_SIZE> bezout_binary(bitset<MAX_SIZE>& e, const bitset<MAX_SIZE>& p, const bitset<MAX_SIZE>& q, bitset<MAX_SIZE>& d)
{
    auto start = std::chrono::high_resolution_clock::now();

    bitset<MAX_SIZE> g(1);

    bitset<MAX_SIZE> p_1 = subtract_binary(p, bitset<MAX_SIZE>(1));
    bitset<MAX_SIZE> q_1 = subtract_binary(q, bitset<MAX_SIZE>(1));
    bitset<MAX_SIZE> phi_n = mul_binary(p_1, q_1);
    bitset<MAX_SIZE> temp_phi_n = phi_n;
    // in ed + b.phi_n = gcd (e, phi_n)  e->x, phi_n -> y
    
    while (e[0] == 0 && phi_n[0] == 0)
    {
        e >>= 1; // x = x/2
        if (e[MAX_SIZE - 2] == 1)
            e[MAX_SIZE - 1] = 1;
        phi_n >>= 1; // y = y/2
        if (phi_n[MAX_SIZE - 2] == 1)
            phi_n[MAX_SIZE - 1] = 1;
        g <<= 1; // g = g * 2
    }

    bitset<MAX_SIZE> u = e;
    bitset<MAX_SIZE> v = phi_n;
    bitset<MAX_SIZE> A(1), B(0), C(0), D(1);
    
    while (compare_binary(u, bitset<MAX_SIZE>(0)) == 1) // u > 0 => sign = 0 and have any bit 1 
    {
        while (u[0] == 0)
        {
            u >>= 1;
            if (u[MAX_SIZE - 2] == 1)
                u[MAX_SIZE - 1] = 1;

            if (A[0] == 0 && B[0] == 0)
            {
                A >>= 1; // A=A/2
                if (A[MAX_SIZE - 2] == 1)
                    A[MAX_SIZE - 1] = 1; 
                B >>= 1; // B=B/2
                if (B[MAX_SIZE - 2] == 1)
                    B[MAX_SIZE - 1] = 1;
            }
            else
            {
                A = add_binary(A, phi_n);
                A >>= 1;
                if (A[MAX_SIZE - 2] == 1)
                    A[MAX_SIZE - 1] = 1;
                B = subtract_binary(B, e);
                B >>= 1;
                if (B[MAX_SIZE - 2] == 1)
                    B[MAX_SIZE - 1] = 1;
            }
        }


        while (v[0]==0)
        {
            v >>= 1;
            if (v[MAX_SIZE - 2] == 1)
                v[MAX_SIZE - 1] = 1;
            if (C[0] == 0 && D[0] == 0)
            {
                C >>= 1;
                if (C[MAX_SIZE - 2] == 1)
                    C[MAX_SIZE - 1] = 1;
                D >>= 1;
                if (D[MAX_SIZE - 2] == 1)
                    D[MAX_SIZE - 1] = 1;
            }
            else
            {
                C = add_binary(C, phi_n);
                C >>= 1;
                if (C[MAX_SIZE - 2] == 1)
                    C[MAX_SIZE - 1] = 1;
                D = subtract_binary(D, e);
                D >>= 1;
                if (D[MAX_SIZE - 2] == 1)
                    D[MAX_SIZE - 1] = 1;
            }
        }

        if (compare_binary(u, v) == -1)
        {
            v = subtract_binary(v, u);
            C = subtract_binary(C, A);
            D = subtract_binary(D, B);
        }
        else
        {
            u = subtract_binary(u, v);
            A = subtract_binary(A, C);
            B = subtract_binary(B, D);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        if (duration > 60)
        {
            cerr << "Time out!";
            exit(1);
        }
    }
    if (C[MAX_SIZE - 1] == 1)
        C = add_binary(C, temp_phi_n);
    d = C; 
    v = mul_binary(v, g);
    return v;
}

int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b)
{
    if (a[MAX_SIZE - 1] > b[MAX_SIZE - 1]) // sign a = 1 => a < 0 ; sign b = 0 => b >= 0 => a < b
        return -1;
    if (a[MAX_SIZE - 1] < b[MAX_SIZE - 1])
        return 1;

    for (int i = a.size() - 2; i >= 0; --i)
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