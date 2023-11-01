#include<bitset>
#include<iostream>
#include<string>
#include<fstream>
#include<chrono>

#define MAX_SIZE 513

using namespace std;

int Primes[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
73, 79, 83, 89, 97, 101, 103, 107, 109, 113,
127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
179, 181, 191, 193, 197, 199, 211, 223, 227, 229,
233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
283, 293, 307, 311, 313, 317, 331, 337, 347, 349,
353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
419, 421, 431, 433, 439, 443, 449, 457, 461, 463,
467, 479, 487, 491, 499, 503, 509, 521, 523, 541 }; //  the first 100 prime numbers

//string input_file_name = "test_19.inp";
//string output_file_name = "test_19.out";

bitset<4> hex_char_to_bin(char c);
bitset<MAX_SIZE> hex_str_to_bin(const string& hex);
bitset<MAX_SIZE> add_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
int compare_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> subtract_binary(const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& b);
bitset<MAX_SIZE> modulo(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> mul_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& y, const bitset<MAX_SIZE>& n);
bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n);
bool check_prime_by_Miller_Rabin(const bitset<MAX_SIZE>& n);

int main(int argc, char* argv[])
{
    //auto start = std::chrono::high_resolution_clock::now();
    
    if (argc != 3)
    {
        cerr << "Usage: $ ./main test_XX.inp test_XX.out" << endl;
        return 1;
    }

    ifstream input_file(argv[1]);
    
    //ifstream input_file(input_file_name);

    string hex_number;

    if (input_file.is_open())
    {
        char c = '\0';
        while (!input_file.eof())
        {
            input_file >> c;
            hex_number = c + hex_number;
        }
        hex_number.erase(0, 1);
        //cout << "Hex:" << hex_number << endl;
        input_file.close();
    }
    else
    {
        cout << "Can't open file " << argv[1] << endl;
        return 1;
    }

    bitset<MAX_SIZE> binary = hex_str_to_bin(hex_number);
    //cout << "Binary: " << binary << endl;
    
    ofstream output_file(argv[2]);
    //ofstream output_file(output_file_name);

    if (output_file.is_open())
    {
        bool check = check_prime_by_Miller_Rabin(binary);
        if (check)
        {
            //cout << "1\n";
            output_file << "1";
        }
        else
        {
            //cout << "0\n";
            output_file << "0";
        }
        output_file.close();
    }
    else {
        cout << "Can't open file " << argv[2] << endl;
        return 1;
    }

    /*auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    cout << "Time: " << duration << " s" << endl;*/
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
    case 'F': return bitset<4>("1111");
    }
    return NULL; // Default case
}

bitset<MAX_SIZE> hex_str_to_bin(const string& hex) 
{
    bitset<MAX_SIZE> bin;
    int length = hex.length() * 4; 
    int bitset_size = min(length, MAX_SIZE);

    for (int i = 0; i < bitset_size; ++i) 
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

bitset<MAX_SIZE> power_mod(const bitset<MAX_SIZE>& x, const bitset<MAX_SIZE>& a, const bitset<MAX_SIZE>& n)
{

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
    }

    return result;
}

bool check_prime_by_Miller_Rabin(const bitset<MAX_SIZE>& n) 
{
    if (n[0] == 0 && n.count() > 1) return false; // n is even and n > 2 

    // cal s and d in formula n-1 = 2^s.d
    int s = 0;
    int count = 0;
    for (int i = 0; i < n.size(); ++i)
    {
        if (n[i] == 1) count++;

        if (count == 2) break;
        else
        {
            s++;
        }
    }

    bitset<MAX_SIZE> d = n >> s;

    bitset<MAX_SIZE> n_1 = subtract_binary(n, bitset<MAX_SIZE>(1));
    srand(time(0));
   
    auto start = std::chrono::high_resolution_clock::now();

    for (int j = 0; j < 4; ++j) 
    {
        int a_int = rand() % 100; 
        bitset<MAX_SIZE> a(Primes[a_int]);


        bitset<MAX_SIZE> x = power_mod(a, d, n);
        bitset<MAX_SIZE> y;

        for (int i = 0; i < s; ++i) 
        {
            y = mul_mod(x, x, n);
            if (y == 1 && x != 1 && x != n_1) 
            {
                return false;
            }
            x = y;
        }

        if (y != 1) 
        {
            return false;
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
        if (duration > 60) 
        {   
            cout << "\nMore than 60s" << endl;
            exit(-1);
        }
    }

    return true;
}