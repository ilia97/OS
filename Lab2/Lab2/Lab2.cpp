// Lab2.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <math.h>
#include <vector>
#include <time.h>

using namespace std;

// Функция для нахождения первых m простых чисел.
long* GetArrayOfPrimes(unsigned long m)
{
	long* arr = new long[m];
	arr[0] = 2;
	arr[1] = 3;

	long k = 5;
	for (long i = 2; i < m; )
	{
		bool isPrime = true;
		for (int j = 0; j < i; j++)
		{
			if (k % arr[j] == 0)
			{
				isPrime = false;
				break;
			}
		}
		if (isPrime)
		{
			arr[i] = k;
			i++;
		}

		k += 2;
	}

	return arr;
}

bool IsBiggerOrEqual(vector<int> x, vector<int> y)
{
	if (x.size() > y.size())
		return true;
	else if (x.size() < y.size())
		return false;
	else
	{
		// Так как числа записаны в векторе задом наперёд, то проходим с конца вектора.
		for (int i = x.size() - 1; i >= 0; i--)
		{
			if (x[i] > y[i])
				return true;
			else if (x[i] < y[i])
				return false;
		}
		return true;
	}
}

vector<int> LongMultiply(vector<int> x, vector<int> y)
{
	// Если одно из чисел 0, то и произведение 0.
	vector<int> zero = { 0 };
	if ((x.size() == 1 && x[0] == 0) || (y.size() == 1 && y[0] == 0))
		return zero;

	vector<int> z;
	for (int i = 0; i < x.size(); i++)
	{
		for (int j = 0; j < y.size(); j++)
		{
			if (i + j >= z.size())
				z.push_back(x[i] * y[j]);
			else
				z[i + j] += x[i] * y[j];
		}
	}

	for (int i = 0; i < z.size(); i++)
	{
		if (i == z.size() - 1)
		{
			if (z[i] / 10 > 0)
				z.push_back(z[i] / 10);
		}
		else
		{
			z[i + 1] += z[i] / 10;
		}
		z[i] %= 10;
	}

	return z;
}

vector<int> LongSum(vector<int> x, vector<int> y)
{
	vector<int> z;
	unsigned int length = x.size() > y.size() ? y.size() : x.size();
	int carry = 0;
	unsigned int i = 0;
	for (; i < length; i++)
	{
		z.push_back((x[i] + y[i] + carry) % 10);

		if (x[i] + y[i] + carry >= 10)
			carry = 1;
		else
			carry = 0;
	}

	// Вариант, когда длина первого вектора больше, чем длина второго.
	if (x.size() > length)
	{
		for (; i < x.size(); i++)
		{
			z.push_back((x[i] + carry) % 10);
			if (x[i] + carry >= 10)
			{
				carry = 1;
			}
			else
			{
				carry = 0;
			}
		}
	}
	// Вариант, когда длина второго вектора больше, чем длина первого.
	else if (y.size() > length)
	{
		for (; i < y.size(); i++)
		{
			z.push_back((y[i] + carry) % 10);
			if (y[i] + carry >= 10)
			{
				carry = 1;
			}
			else
			{
				carry = 0;
			}
		}
	}
	// Вариант, когда длины векторов одинаковы.
	// Проверяем на перенос.
	if (carry > 0)
		z.push_back(carry);

	return z;
}

long LongDigitSum(vector<int> x)
{
	long digitSum = 0;
	for (int i = 0; i < x.size(); i++)
	{
		digitSum += x[i];
	}
	return digitSum;
}

// х обязательно больше либо равно y!
vector<int> LongSubstraction(vector<int> x, vector<int> y)
{
	vector<int> z;

	for (int i = 0; i < x.size(); i++)
	{
		if (i < y.size())
		{
			if (x[i] >= y[i])
				z.push_back(x[i] - y[i]);
			else
			{
				z.push_back(x[i] - y[i] + 10);
				x[i + 1] -= 1;
			}
		}
		else
		{
			if (x[i] >= 0)
				z.push_back(x[i]);
			else
			{
				z.push_back(x[i] + 10);
				x[i + 1] -= 1;
			}
		}
	}

	// В случае, когда в конце остались 0, их надо удалить.
	while (z.size() > 1 && z[z.size() - 1] == 0 )
		z.pop_back();

	return z;
}

vector<int> LongMod(vector<int> x, vector<int> y)
{
	vector<int> z;
	for (int i = x.size() - 1; i >= 0; i--)
	{
		vector<int> ten = { 0, 1 };
		vector<int> nextDigit = { x[i] };
		z = LongMultiply(z, ten);
		z = LongSum(z, nextDigit);
		while (IsBiggerOrEqual(z, y))
		{
			z = LongSubstraction(z, y);
		}
	}
	while (IsBiggerOrEqual(z, y))
	{
		z = LongSubstraction(z, y);
	}
	return z;
}

vector<int> LongDivision(vector<int> x, vector<int> y)
{
	vector<int> z;
	vector<int> result;
	vector<int> one = { 1 };
	for (int i = x.size() - 1; i >= 0; i--)
	{
		vector<int> ten = { 0, 1 };
		result = LongMultiply(result, ten);

		vector<int> nextDigit = { x[i] };
		z = LongMultiply(z, ten);
		z = LongSum(z, nextDigit);
		while (IsBiggerOrEqual(z, y))
		{
			z = LongSubstraction(z, y);
			result = LongSum(result, one);
		}
	}
	while (IsBiggerOrEqual(z, y))
	{
		z = LongSubstraction(z, y);
	}
	return result;
}

vector<int> LongPowMod(vector<int> x, vector<int> p, vector<int> m)
{
	int size = p.size();
	int firstDigit = p[0];
	vector<int> two = { 2 };
	vector<int> one = { 1 };
	if (size == 1 && firstDigit == 0)
	{
		vector<int> z = { 1 };
		return z;
	}
	if (size == 1 && firstDigit == 1)
	{
		return LongMod(x, m);
	}
	// (x%m * x%m)%m
	if (size == 1 && firstDigit == 2)
	{
		return LongMod(LongMultiply(LongMod(x, m), LongMod(x, m)), m);
	}
	else if (firstDigit % 2 == 0)
		return LongPowMod(LongPowMod(x, LongDivision(p, { 2 }), m), { 2 }, m);
	else
		return LongMod(LongMultiply(LongPowMod(LongPowMod(x, LongDivision(LongSubstraction(p, { 1 }), { 2 }), m), { 2 }, m), x), m);
}

vector<int> LongRandom(vector<int> to)
{
	srand((unsigned)time(NULL));
	vector<int> result;
	for (int i = 0; i < to.size() - 1; i++)
	{
		int r = rand();
		result.push_back(r % 10);
	}
	int r = rand();
	result.push_back(r % to[to.size() - 1]);

	// Удаляем нули в первых разрядах, если они есть.
	while (result[result.size() - 1] == 0)
	{
		result.pop_back();
	}
	return result;
}

unsigned long PowMod(unsigned long number, unsigned long pow, unsigned long mod)
{
	if (pow == 0)
		return 0;
	if (pow == 1)
		return number % mod;
	if (pow == 2)
		return ((number % mod) * (number % mod)) % mod;
	else if (pow % 2 == 1)
		return ((PowMod((PowMod(number, (pow - 1) / 2, mod) % mod), 2, mod) % mod) * (number % mod)) % mod;
	else
		return (PowMod((PowMod(number, pow / 2, mod) % mod), 2, mod) % mod) % mod;
}

bool IsPrimeMillerRabin(vector<int> number, int iterationCount)
{
	// Отсеиваем чётные числа кроме 2.
	if (number.size() == 1 && number[0] == 2)
		return true;
	if (number[0] % 2 == 0)
		return false;

	vector<int> t = number;
	vector<int> s;
	t = LongSubstraction(t, { 1 });
	while (LongMod(t, { 2 })[0] == 0)
	{
		t = LongDivision(t, { 2 });
		s = LongSum(s, { 1 });
	}

	for (int i = 0; i < iterationCount; i++)
	{
		vector<int> a = LongRandom(LongSubstraction(number, { 2 }));
		vector<int> x = LongPowMod(a, t, number);
		// Если х == 1 || x == n - 1
		vector<int> substr = LongSubstraction(number, x);
		if ((x.size() == 1 && x[0] == 1) || (substr.size() == 1 && substr[0] == 1))
		{
			continue;
		}

		bool k = false;

		// Цикл есть смыл проходить только если s > 1.
		if (IsBiggerOrEqual(s, { 2 }))
		{
			for (vector<int> j = { 0 }; IsBiggerOrEqual(LongSubstraction(s, { 2 }), j); j = LongSum(j, { 1 }))
			{
				x = LongPowMod(x, { 2 }, number);
				if (x.size() == 1 && x[0] == 1)
					return false;
				// Если x == n - 1
				vector<int> substr = LongSubstraction(number, x);
				if (substr.size() == 1 && substr[0] == 1)
				{
					k = true;
					break;
				}
			}
		}
		if (k)
			continue;
		return false;
	}
	return true;
}

vector<int> GetRandomPrime(int bitCount)
{
	srand((unsigned)time(NULL));
	vector<int> z;
	vector<int> one = { 1 };
	vector<int> two = { 2 };

	for (int i = 0; i < bitCount - 1; i++)
	{
		// Генерируем 0 или 1.
		z.push_back(rand() % 2);
	}
	// Обязательно ведущая единица.
	z.push_back(1);

	// Переводим в десятичную систему счисления (k - нужное нам число).
	vector<int> k = { 0 };
	vector<int> powOfTwo = { 1 };
	for (int i = 0; i < z.size(); i++)
	{
		if (z[i] == 1)
			k = LongSum(k, powOfTwo);
		powOfTwo = LongMultiply(powOfTwo, two);
	}

	// Отсеиваем числа по делимости на 2, 3 и 5 (73% чисел).
	while (k[0] % 2 == 0 ||
		k[0] % 5 == 0 ||
		LongDigitSum(k) % 3 == 0 ||
		!IsPrimeMillerRabin(k, 5))
	{
		k = LongSum(k, one);
	}

	return k;
}

//vector<int> ConvertToVector(string x)
//{
//	vector<int> z;
//	for (int i = 0; i < x.length; i++)
//	{
//		z.push_back(x[i]);
//	}
//	return z;
//}

// Примитивный RSA.
__declspec (dllexport) void __stdcall NumberRSAencryption(unsigned long number, unsigned long* result, unsigned long* key, unsigned long* outN)
{
	unsigned long primesCount = sqrt(number) > 5 ? sqrt(number) : 5;
	long* arrayOfPrimes = GetArrayOfPrimes(primesCount);
	unsigned long p = arrayOfPrimes[primesCount - 1];
	unsigned long q = arrayOfPrimes[primesCount - 2];
	unsigned long n = p * q;
	unsigned long eiler = (p - 1) * (q - 1);

	int i = 0;
	while (eiler % arrayOfPrimes[i] == 0)
	{
		i++;
	}
	unsigned long e = arrayOfPrimes[i];

	unsigned long d = 1;
	while ((d * e) % eiler != 1)
		d++;

	unsigned long res = PowMod(number, e, n);
	*result = res;
	*key = d;
	*outN = n;

	delete[] arrayOfPrimes;
}

__declspec (dllexport) void __stdcall NumberRSAdecryption(unsigned long number, unsigned long* key, unsigned long* n, unsigned long* result)
{
	*result = PowMod(number, *key, *n);
}

__declspec (dllexport) void __stdcall LongNumberRSAencryption(vector<int> number, vector<int>* result, vector<int>* key, vector<int>* outN)
{
	vector<int> p = GetRandomPrime(32);
	vector<int> q = GetRandomPrime(32);
	vector<int> n = LongMultiply(p, q);
	vector<int> one = { 1 };
	vector<int> eiler = LongMultiply(LongSubstraction(p, one), LongSubstraction(q, one));

	// Подбираем простое число Ферма.
	vector<int> e = { 3 };
	int i = 1;
	while (LongMod(eiler, e).size() == 1 && LongMod(eiler, e)[0] == 0)
	{
		vector<int> k = LongSubstraction(e, one);
		e = LongSum(LongMultiply(k, k), one);
	}

	// D = ( (ФИ * i) / E ). Тогда всегда D*E=1 (mod ФИ).
	vector<int> d = { 1 };
	vector<int> x = { 1 };
	while (IsBiggerOrEqual(LongMod(LongSum(LongMultiply(eiler, x), { 1 }), e), {1}))
	{
		x = LongSum(x, { 1 });
	}
	d = LongDivision(LongSum(LongMultiply(eiler, x), { 1 }), e);

	vector<int> res = LongPowMod(number, e, n);
	*result = res;
	*key = d;
	*outN = n;
}

__declspec (dllexport) void __stdcall LongNumberRSAdecryption(vector<int> number, vector<int>* key, vector<int>* n, vector<int>* result)
{
	*result = LongPowMod(number, *key, *n);
}

__declspec (dllexport) vector<vector<int>> __stdcall StringRSAencryption(TCHAR str[], unsigned long strLen, vector<int>* key, vector<int>* outN)
{
	vector<vector<int>> result;

	int charNum = 0;

	vector<int> p = GetRandomPrime(32);
	vector<int> q = GetRandomPrime(32);
	vector<int> n = LongMultiply(p, q);
	vector<int> one = { 1 };
	vector<int> eiler = LongMultiply(LongSubstraction(p, one), LongSubstraction(q, one));

	// Подбираем простое число Ферма.
	vector<int> e = { 3 };
	int i = 1;
	while (LongMod(eiler, e).size() == 1 && LongMod(eiler, e)[0] == 0)
	{
		vector<int> k = LongSubstraction(e, one);
		e = LongSum(LongMultiply(k, k), one);
	}

	// D = ( (ФИ * i) / E ). Тогда всегда D*E=1 (mod ФИ).
	vector<int> d = { 1 };
	vector<int> x = { 1 };
	while (IsBiggerOrEqual(LongMod(LongSum(LongMultiply(eiler, x), { 1 }), e), { 1 }))
	{
		x = LongSum(x, { 1 });
	}
	d = LongDivision(LongSum(LongMultiply(eiler, x), { 1 }), e);

	*key = d;
	*outN = n;

	for (int i = 0; i < strLen; i++)
	{
		charNum = str[i];

		// Переводим число в вектор.
		vector<int> z;
		while (charNum > 0)
		{
			z.push_back(charNum % 10);
			charNum /= 10;
		}

		vector<int> numResult = LongPowMod(z, e, n);

		result.push_back(numResult);
	}
	return result;
}

__declspec (dllexport) void __stdcall StringRSAdecryption(vector<vector<int>> str, unsigned long strLen, vector<int>* key, vector<int>* outN, TCHAR result[])
{
	int charNum = 0;
	for (int i = 0; i < strLen; i++)
	{
		vector<int> charNum = str[i];

		vector<int> numResult;
		LongNumberRSAdecryption(charNum, key, outN, &numResult);
		
		// Переводим вектор в число.
		int res = 0;
		for (int i = numResult.size() - 1; i >= 0; i--)
		{
			res *= 10;
			res += numResult[i];
		}
		TCHAR newChar = (TCHAR)res;
		result[i] = newChar;
	}
}
