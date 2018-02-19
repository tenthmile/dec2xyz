#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <gmp.h>
#include <string>
#include <iostream>

class BigInteger {
private:
  mpz_t value;

public:
  BigInteger() {
    mpz_init(value);
  }

  BigInteger(const char *c_string, int base = 10) {
    mpz_init_set_str(value, c_string, base);
  }

  BigInteger(const std::string string, int base = 10) : BigInteger(string.c_str(), base) { }

  BigInteger(const BigInteger &other) {
    mpz_init_set(value, other.value);
  }
  
  ~BigInteger() {
    mpz_clear(value);
  }

  bool operator<(const BigInteger &other) {
    return mpz_cmp(value, other.value) < 0;
  }

  bool operator<(signed long int val) {
    return mpz_cmp_si(value, val) < 0;
  }
  
  bool operator<(unsigned long int val) {
    return mpz_cmp_ui(value, val) < 0;
  }

  bool operator<=(const BigInteger &other) {
    return mpz_cmp(value, other.value) <= 0;
  }

  bool operator<=(signed long int val) {
    return mpz_cmp_si(value, val) <= 0;
  }
  
  bool operator<=(unsigned long int val) {
    return mpz_cmp_ui(value, val) <= 0;
  }

  bool operator>(const BigInteger &other) {
    return mpz_cmp(value, other.value) > 0;
  }

  bool operator>(signed long int val) {
    return mpz_cmp_si(value, val) > 0;
  }
  
  bool operator>(unsigned long int val) {
    return mpz_cmp_ui(value, val) > 0;
  }

  bool operator>=(const BigInteger &other) {
    return mpz_cmp(value, other.value) >= 0;
  }

  bool operator>=(signed long int val) {
    return mpz_cmp_si(value, val) >= 0;
  }
  
  bool operator>=(unsigned long int val) {
    return mpz_cmp_ui(value, val) >= 0;
  }

  bool operator==(const BigInteger &other) {
    return mpz_cmp(value, other.value) == 0;
  }

  bool operator==(signed long int val) {
    return mpz_cmp_si(value, val) == 0;
  }

  bool operator==(unsigned long int val) {
    return mpz_cmp_ui(value, val) == 0;
  }

  BigInteger &operator=(const BigInteger &other) {
    mpz_set(value, other.value);
    return *this;
  }
  
  BigInteger &operator=(mpz_t &mpz_other) {
    mpz_set(value, mpz_other);
    return *this;
  }

  BigInteger &operator=(char *str) {
    mpz_set_str(value, str, 10);
    return *this;
  }

  BigInteger &operator=(std::string str) {
    mpz_set_str(value, str.c_str(), 10);
    return *this;
  }

  BigInteger &operator+=(const BigInteger &other) {
    mpz_add(value, value, other.value);
    return *this;
  }

  BigInteger &operator+=(unsigned long int val) {
    mpz_add_ui(value, value, val);
    return *this;
  }

  BigInteger &operator-=(const BigInteger &other) {
    mpz_sub(value, value, other.value);
    return *this;
  }

  BigInteger &operator-=(unsigned long int val) {
    mpz_sub_ui(value, value, val);
    return *this;
  }

  BigInteger &operator/=(unsigned long int val) {
    mpz_fdiv_q_ui(value, value, val);
    return *this;
  }

  friend unsigned long int operator%(const BigInteger &lhs, unsigned long int mod) {
    size_t wordCount;
    unsigned long int remainder;
    mpz_t r;
    mpz_init(r);
    mpz_mod_ui(r, lhs.value, mod);
    mpz_export(&remainder, &wordCount, 0, sizeof(unsigned long int), 0, 0, r);
    mpz_clear(r);
    return wordCount > 0 ? remainder : 0;
  }

  friend std::ostream &operator<<(std::ostream &out, const BigInteger &bigI) {
    char *c_string = mpz_get_str(0, 10, bigI.value);
    out << c_string;
    delete c_string;
    return out;
  }
};

#endif

