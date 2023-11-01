#include<iostream>
#include<fstream>
#include<string>
#include<chrono>
#include<cstdlib>
#include<ctime>
#include<algorithm>

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

string input_file_name = "test_09.inp";
string output_file_name = "test_09.out";

const char* hex_char_to_bin(char c);
bool check_prime(int n);
int bin_to_dec(string n);
string hex_str_to_bin_str(const string& hex);
bool check_prime_by_Miller_Rabin(const string& n);
string dec_to_bin(int n);
string modulo(const string& x, const string& n);
string add_binary(const string& x, const string& y);
string subtract_binary(const string& a, const string& b);
int compare_binary(const string& a, const string& b);
string mul_mod(const string& x, const string& y, const string& n);
string power_mod(const string& x, const string& a, const string& n);

int main(int argc, char* argv[])
{
	auto start = std::chrono::high_resolution_clock::now();
	srand((unsigned int)time(NULL));

	/*if (argc != 3)
	{
		cerr << "Usage: $ ./main test_XX.inp test_XX.out" << endl;
		return 1;
	}*/

	//ifstream input_file(argv[1]);

	ifstream input_file(input_file_name);

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
		cout << hex_number << endl;
		input_file.close();
	}
	else
	{
		//cout << "Can't open file " << argv[1] << endl;
		cout << "Can't open file " << input_file_name << endl;
		return 1;
	}

	string binary = hex_str_to_bin_str(hex_number);

	//ofstream output_file(argv[2]);
	ofstream output_file(output_file_name);

	if (output_file.is_open())
	{
		bool check;
		if (binary.size() > 20) check = check_prime_by_Miller_Rabin(binary);
		else
		{
			check = check_prime(bin_to_dec(binary));
		}

		if (check)
		{
			cout << "1\n";
			output_file << "1";
		}
		else
		{
			cout << "0\n";
			output_file << "0";
		}
		output_file.close();
	}
	else {
		//cout << "Can't open file " << argv[2] << endl;
		cout << "Can't open file " << output_file_name << endl;
		return 1;
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
	cout << "Time: " << duration << " s" << std::endl;

	return 0;
}


const char* hex_char_to_bin(char c)
{
	switch (toupper(c))
	{
	case '0': return "0000";
	case '1': return "0001";
	case '2': return "0010";
	case '3': return "0011";
	case '4': return "0100";
	case '5': return "0101";
	case '6': return "0110";
	case '7': return "0111";
	case '8': return "1000";
	case '9': return "1001";
	case 'A': return "1010";
	case 'B': return "1011";
	case 'C': return "1100";
	case 'D': return "1101";
	case 'E': return "1110";
	case 'F': return "1111";
	}
	return NULL;
}

string hex_str_to_bin_str(const string& hex)
{
	string bin;
	for (unsigned i = 0; i != hex.length(); ++i)
		bin += hex_char_to_bin(hex[i]);
	while (bin.size() >= 1 && bin[0] == '0')
		bin.erase(bin.begin());
	return bin;
}


bool check_prime_by_Miller_Rabin(const string& n)
{
	// perform n-1 = 2^s.d
	int count = 0;
	string d = n;
	int s = 0;

	for (int i = n.size() - 1; i >= 0; --i)
	{
		if (n[i] == '1') count++;

		if (count == 2) break;
		else
		{
			d.erase(d.size() - 1, 1);
			s++;
		}
	}

	// cal n-1 
	string n_1 = subtract_binary(n, "1");

	for (int j = 0; j < 4; ++j)
	{
		int random_index = rand() % 100;
		cout << "random:" << random_index << endl;
		int a_int = Primes[random_index];
		string a = dec_to_bin(a_int);

		string x = power_mod(a, d, n);
		string y;

		for (int i = 0; i < s; ++i)
		{
			y = mul_mod(x, x, n);
			if (y == "1" && x != "1" && x != n_1)
				return false;
			x = y;
		}
		if (y != "1") return false;
	}
	return true;
}

string dec_to_bin(int n)
{
	string binary;
	while (n > 0)
	{
		binary = (char)((n % 2) + '0') + binary;
		n = n / 2;
	}
	return binary;
}

string modulo(const string& x, const string& n)
{
	int compare = compare_binary(x, n);
	if (compare == 0) return "0"; //x=n
	if (compare == -1) return x;  // x<n

	int maxLength = max(x.length(), n.length());
	string adjusted_x = string(maxLength - x.length(), '0') + x;
	string adjusted_n = string(maxLength - n.length(), '0') + n;

	while (compare_binary(adjusted_x, adjusted_n) == 1 || compare_binary(adjusted_x, adjusted_n) == 0) // x > n then x = x-n
	{
		adjusted_x = subtract_binary(adjusted_x, adjusted_n);
	}

	return adjusted_x;
}

string subtract_binary(const string& a, const string& b)
{
	if (a == b) return "0";

	int maxLength = max(a.size(), b.size());
	string adjusted_a = string(maxLength - a.size(), '0') + a;
	string adjusted_b = string(maxLength - b.size(), '0') + b;

	string difference = "";
	int borrow = 0;

	for (int i = maxLength - 1; i >= 0; --i)
	{
		int bitA = adjusted_a[i] - '0';
		int bitB = adjusted_b[i] - '0';

		int subtraction = bitA - bitB - borrow;
		if (subtraction < 0) {
			subtraction += 2;
			borrow = 1;
		}
		else {
			borrow = 0;
		}

		difference = to_string(subtraction) + difference;
	}

	while (difference.size() > 1 && difference[0] == '0')
	{
		difference.erase(difference.begin());
	}

	return difference;
}

int compare_binary(const string& a, const string& b)
{
	int maxLength = max(a.size(), b.size());
	string adjustedA = string(maxLength - a.size(), '0') + a;
	string adjustedB = string(maxLength - b.size(), '0') + b;

	// Compare from left to right
	for (int i = 0; i < maxLength; ++i) {
		if (adjustedA[i] < adjustedB[i]) {
			return -1; // a < b
		}
		else if (adjustedA[i] > adjustedB[i]) {
			return 1;  // a > b
		}
	}

	return 0; // a == b
}

string add_binary(const string& x, const string& y)
{
	string result = "";
	int carry = 0;

	int i = x.size() - 1;
	int j = y.size() - 1;

	while (i >= 0 || j >= 0 || carry) {
		int sum = carry;

		if (i >= 0) {
			sum += (x[i] - '0');
			i--;
		}

		if (j >= 0) {
			sum += (y[j] - '0');
			j--;
		}

		carry = sum / 2;
		sum %= 2;

		result.push_back(sum + '0');
	}

	reverse(result.begin(), result.end());
	return result;
}


string mul_mod(const string& x, const string& y, const string& n)
{
	string temp_x = modulo(x, n);
	string result = "0";

	// y = y_{b-1}y_{b-2}...y_1y_0

	if (y[y.size() - 1] == '1') result = temp_x; // y0 = 1 => P = x

	for (int i = y.size() - 2; i >= 0; --i)
	{
		temp_x = temp_x + "0";
		temp_x = modulo(temp_x, n); // x = x*2 (mod n)

		if (y[i] == '1')
		{
			result = add_binary(result, temp_x);
			result = modulo(result, n);
			// y_i = 1 => P = P + x (mod n) 
		}
	}

	return result;
}

string power_mod(const string& x, const string& a, const string& n)
{
	string result = "1";

	//for (int i = a.size() - 1; i >= 0; --i)
	for (int i = 0; i < a.size(); ++i)
	{
		// y = y^2 (mod n)
		result = mul_mod(result, result, n);

		if (a[i] == '1')
		{
			result = mul_mod(result, x, n); // y = y*x (mod n)
		}

	}

	return result;
}

int bin_to_dec(string n)
{
	string num = n;
	int dec_value = 0;

	// Initializing base value to 1, i.e 2^0 
	int base = 1;

	int len = num.length();
	for (int i = len - 1; i >= 0; i--) {
		if (num[i] == '1')
			dec_value += base;
		base = base * 2;
	}

	return dec_value;
}

bool check_prime(int n)
{
	if (n <= 1)
	{
		return false;
	}

	if (n == 2)
	{
		return true; // 2 is a prime number
	}

	if (n % 2 == 0)
	{
		return false; // Even numbers greater than 2 are not prime
	}

	// Check for divisibility from 3 to square root of n
	for (int i = 3; i * i <= n; i += 2)
	{
		if (n % i == 0)
		{
			return false; // n is divisible by i, so it's not prime
		}
	}

	return true;
}
