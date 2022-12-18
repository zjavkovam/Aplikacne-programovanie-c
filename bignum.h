#pragma once

// here you can include whatever you want :)
#include <string>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#define SUPPORT_DIVISION 1 // define as 1 when you have implemented the division
#define SUPPORT_IFSTREAM 0 // define as 1 when you have implemented the input >>

// if you do not plan to implement bonus, just delete those lines
class BigNum final
{
public:
    // constructors
    BigNum(){
        number.push_back('+');
        number.push_back('0');
    }
    BigNum(int64_t n){
        bool negative = false;
        if(n<0){
            negative = true;
            n *= -1;
        }

        if(n==0){
            number.push_back('0');
        }

        while(n>0){
            char digit = '0'+(n%10);
            number.push_back(static_cast<char>(digit));
            n/=10;
        }
        std::reverse(number.begin(), number.end());

        if(negative){
            number.insert(number.begin(),'-');
        }
        else{
            number.insert(number.begin(),'+');
        }
    }
    explicit BigNum(const std::string& str){
        if(str.empty()){
            throw std::invalid_argument( "received empty input");
        }

        bool started_0 = true;
        for (char digit: str) {
            //ignore all 0
            if((digit < '0' or digit > '9') and digit!='+' and digit != '-'){
                throw std::invalid_argument( "received wrong input");
            }
            if(digit != '0' and digit!='+' and digit != '-'){
                started_0 = false;
            }
            if(!started_0 or digit=='+' or digit == '-') {
                number.push_back(digit);
            }
        }

        //if in string was only zero, put one back
        if((!str.empty() and number.size()==1 and (number[0]=='+' or number[0]=='-')) or number.empty()){
            number.push_back('0');
        }

        if(number[0]!='-' and number[0]!='+'){
            number.insert(number.begin(),'+');
        }

        //change -0 to +0
        if(number[0]=='-' and number.size()==2 and number[1]=='0'){
            number[0]='+';
        }

        if(std::count(number.begin(), number.end(), '-')>1){
            throw std::invalid_argument( "received wrong input");
        }
        if(std::count(number.begin(), number.end(), '+')>1){
            throw std::invalid_argument( "received wrong input");
        }
    }

    // copy
    BigNum(const BigNum& other) = default;
    BigNum& operator=(const BigNum& rhs) = default;

    // unary operators
    const BigNum& operator+() const{
        return *this;
    }
    BigNum operator-() const{
        std::string string_num(number.begin(), number.end());
        BigNum new_number = BigNum(string_num);
        if(new_number.number[0] == '-'){
            new_number.number[0] = '+';
        }
        else{
            new_number.number[0] = '-';
        }
        return new_number;
    }

    // binary arithmetics operators
    BigNum& operator+=(const BigNum& rhs) {
        BigNum result = *this+rhs;
        this->number = result.number;
        return *this;
    }
    BigNum& operator-=(const BigNum& rhs){
        BigNum result = *this-rhs;
        this->number = result.number;
        return *this;
    }
    BigNum& operator*=(const BigNum& rhs){
        BigNum result = *this*rhs;
        this->number = result.number;
        return *this;
    }

#if SUPPORT_DIVISION == 1
    BigNum& operator/=(const BigNum& rhs){
        BigNum result = *this/rhs;
        this->number = result.number;
        return *this;
    } // bonus
    BigNum& operator%=(const BigNum& rhs){
        BigNum result = *this%rhs;
        this->number = result.number;
        return *this;
    }// bonus
#endif

private:
    std::vector<char> number;

    std::vector<char> delete_zeros(std::vector<char> number){
        //delete all zeros infornt of number
        while(number.size()>2){
            if(number[1] == '0'){
                number.erase(number.begin()+1);
            }
            else if(number[1]!='+' and number[1]!='-'){
                break;
            }
        }
        return number;
    }
    std::string addition(const std::vector<char> &lhs, const std::vector<char> &rhs){
        std::vector<char> result;
        int size = static_cast<int>(std::max(lhs.size(),rhs.size()))-1;
        int lhs_size = static_cast<int>(lhs.size()), rhs_size = static_cast<int>(rhs.size());

        char digit;
        int carry = 0;
        for(int i=1; i<=size; i++){
            digit = '0'+carry;
            carry = 0;
            //add digit from first number, if exists
            if(lhs_size-i>0){
                digit += (lhs[lhs_size-i]-'0');
            }
            //add digit from second number, if exists
            if(rhs_size-i>0){
                digit += (rhs[rhs_size-i]-'0');
            }
            //if sum was bigger then 10
            if(digit > '9'){
                digit -= 10;
                carry = 1;
            }
            result.push_back(digit);
        }
        if(carry==1){
            result.push_back('1');
        }

        std::string string_res(result.begin(), result.end());
        std::reverse(string_res.begin(), string_res.end());
        return string_res;
    }
    std::string subtraction(const std::vector<char> &lhs, const std::vector<char> &rhs){
        std::vector<char> result;
        int lhs_size = static_cast<int>(lhs.size()), rhs_size = static_cast<int>(rhs.size());

        //longer number is always lhs
        char digit;
        int carry = 0, subtrahend;
        for(int i=1; i<lhs_size; i++){
            //lhs digit
            digit = (lhs[lhs_size-i]);

            //substract, if rhs is long enough
            if(rhs_size-i>0){
                subtrahend = rhs[rhs_size-i]+ carry - '0';
            }
                //if doesnt, supstrahend is carry, if there is any
            else{
                subtrahend = carry;
            }
            if(digit-subtrahend >= '0'){
                digit-=subtrahend;
                carry = 0;
            }
                //if substraction is less then 0
            else{
                digit = ('9'-((subtrahend+'0')-lhs[lhs_size-i]))+1;
                carry=1;
            }
            result.push_back(digit);
        }

        std::string string_res(result.begin(), result.end());
        std::reverse(string_res.begin(), string_res.end());
        return string_res;

    }
    std::string multiplication(const std::vector<char> &lhs, const std::vector<char> &rhs){
        int lhs_size = static_cast<int>(lhs.size()), rhs_size = static_cast<int>(rhs.size());
        int result_index;
        std::string result((lhs_size+rhs_size-2),'0');

        for(int multiplied = rhs_size-1; multiplied>=1; multiplied--){
            //index of digit in number, starts from left
            result_index = (lhs_size+rhs_size-3) - (rhs_size-(multiplied+1));

            //multiply with current digit of multiplied
            for(int multiplicant = lhs_size-1; multiplicant>=1; multiplicant--){
                int digit = (lhs[multiplicant]-'0') * (rhs[multiplied]-'0') + result[result_index]-'0';
                result[result_index] = digit%10 + '0';
                result[result_index-1] += digit/10;
                result_index--;
            }
        }

        return result;
    }
    std::string division(const std::vector<char> &lhs, const std::vector<char> &rhs) {
        std::vector<char> result, divisor = rhs, dividend_all = lhs;
        dividend_all.erase(dividend_all.begin());

        //if lhs is less then rhs
        if(compare(lhs,rhs)){
            result.push_back('0');
            std::string string_res(result.begin(), result.end());
            return string_res;
        }

        int quotient = 0;
        std::vector<char> dividend(dividend_all.begin(), dividend_all.begin() + rhs.size() - 1);
        dividend.insert(dividend.begin(), '+');
        dividend_all.erase(dividend_all.begin(), dividend_all.begin() + rhs.size() - 2);

        while (!dividend_all.empty()) {
            dividend_all.erase(dividend_all.begin());

            //substracts divisor from dividend while it can
            while (true) {
                dividend = this->delete_zeros(dividend);
                //if dividend is less then divisor end cycle
                if (compare(dividend, divisor)) {
                    break;
                }
                std::string difference = this->subtraction(dividend, divisor);
                std::vector<char> difference_vec(difference.begin(), difference.end());
                //update dividend
                dividend = difference_vec;
                dividend.insert(dividend.begin(), '+');
                quotient++;
            }

            //push back digit
            dividend.push_back(dividend_all[0]);
            result.push_back('0' + quotient);
            quotient = 0;
        }

        std::string string_res(result.begin(), result.end());
        return string_res;
    }
    bool compare(const std::vector<char> &lhs, const std::vector<char> &rhs) const{
        //check if lhs is less than rhs
        if(lhs == rhs or lhs.size() > rhs.size()){
            return false;
        }

        else if(lhs.size() < rhs.size()){
            return true;
        }

            //check digit if length of both numbers is same
        else{
            int index = 1;
            while(lhs[index] == rhs[index] and index<static_cast<int>(lhs.size())){
                index++;
            }
            if(lhs[index] < rhs[index]){
                return true;
            }
            else{
                return false;
            }
        }
    }

    friend BigNum operator+(BigNum lhs, const BigNum& rhs);
    friend BigNum operator-(BigNum lhs, const BigNum& rhs);
    friend BigNum operator*(BigNum lhs, const BigNum& rhs);
    friend bool operator==(const BigNum& lhs, const BigNum& rhs);
    friend bool operator!=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<(const BigNum& lhs, const BigNum& rhs);
    friend bool operator>(const BigNum& lhs, const BigNum& rhs);
    friend bool operator<=(const BigNum& lhs, const BigNum& rhs);
    friend bool operator>=(const BigNum& lhs, const BigNum& rhs);
    friend std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs);
    friend BigNum operator/(BigNum lhs, const BigNum& rhs);
    friend BigNum operator%(BigNum lhs, const BigNum& rhs);

    // here you can add private data and members, but do not add stuff to
    // public interface, also you can declare friends here if you want
};

BigNum operator+(BigNum lhs, const BigNum& rhs){
    std::string string_res;

    /* + - = substraction
     * - - = addition
     * - + = substraction
     * - + = addition
     */

    if(lhs.number[0]=='+' and rhs.number[0]=='+') {
        string_res += "+" + (lhs.addition(lhs.number, rhs.number));
    }
    else if(lhs.number[0]=='-' and rhs.number[0]=='-'){
        string_res += "-" + (lhs.addition(lhs.number, rhs.number));
    }
    else if(lhs.number[0]=='-' and rhs.number[0]=='+'){
        std::string lhs_string(lhs.number.begin(), lhs.number.end());
        if(lhs.compare(lhs.number,rhs.number)) {
            string_res += '+' + (lhs.subtraction(rhs.number, lhs.number));
        }
        else{
            string_res += '-' + (lhs.subtraction(lhs.number,rhs.number));
        }
    }
    else{
        if(!lhs.compare(lhs.number,rhs.number)) {
            string_res += '+' + (lhs.subtraction(lhs.number, rhs.number));
        }
        else{
            string_res += '-' + (lhs.subtraction(rhs.number,lhs.number));
        }
    }

    BigNum res(string_res);
    return res;
}
BigNum operator-(BigNum lhs, const BigNum& rhs){
    std::string string_res;

    /* + + = substraction
    * - - = substraction
    * - + = addition
    * - - = addition
    */

    if(lhs == rhs){
        string_res = '0';
    }

    else if(lhs.number[0] == '+' and rhs.number[0] == '+') {
        if(lhs.compare(lhs.number,rhs.number)){
            string_res = "-" + lhs.subtraction(rhs.number, lhs.number);
        }
        else{
            string_res = "+" + lhs.subtraction(lhs.number, rhs.number);
        }
    }

    else if(lhs.number[0]=='-' and rhs.number[0]=='-'){
        if(lhs.compare(lhs.number,rhs.number)){
            string_res += "+" + (lhs.subtraction(rhs.number, lhs.number));
        }
        else{
            string_res += "-" + (lhs.subtraction(lhs.number, rhs.number));
        }
    }

    else if(lhs.number[0]=='-' and rhs.number[0]=='+'){
        string_res += "-" + (lhs.addition(lhs.number, rhs.number));
    }

    else{
        string_res += "+" + (lhs.addition(lhs.number, rhs.number));
    }

    BigNum res(string_res);
    return res;
}
BigNum operator*(BigNum lhs, const BigNum& rhs){
    std::string result_str = lhs.multiplication(lhs.number, rhs.number);

    if((lhs.number[0]=='-' and rhs.number[0]=='+') or (lhs.number[0]=='+' and rhs.number[0]=='-')){
        result_str = '-' + result_str;
    }
    else{
        result_str = '+' + result_str;
    }
    BigNum result(result_str);
    return result;
}

#if SUPPORT_DIVISION == 1
BigNum operator/(BigNum lhs, const BigNum& rhs){
    //cant divide by 0
    if(rhs.number[1]=='0'){
        throw std::invalid_argument( "division by zero");
    }

    std::string result_str = lhs.division(lhs.number,rhs.number);

    if((lhs.number[0]=='-' and rhs.number[0]=='+') or (lhs.number[0]=='+' and rhs.number[0]=='-')){
        result_str = '-' + result_str;
    }
    else{
        result_str = '+' + result_str;
    }

    BigNum res(result_str);
    return res;
}
BigNum operator%(BigNum lhs, const BigNum& rhs){
    //modulo = lhs + (rhs * division)
    if(rhs.number[1]=='0'){
        throw std::invalid_argument( "division by zero");
    }

    std::string div =  lhs.division(lhs.number,rhs.number);
    //std::cout<<div;
    BigNum result{div};
    result*=rhs;

    if((lhs.number[0]=='-' and result.number[0]=='+') or (lhs.number[0]=='+' and result.number[0]=='-')) {
        result = lhs + result;
    }
    else{
        result = lhs - result;
    }
    return result;
} // bonus
#endif

// alternatively you can implement
// std::strong_ordering operator<=>(const BigNum& lhs, const BigNum& rhs);
// idea is, that all comparison should work, it is not important how you do it
bool operator==(const BigNum& lhs, const BigNum& rhs){
    if(lhs.number == rhs.number){
        return true;
    }
    return false;
}
bool operator!=(const BigNum& lhs, const BigNum& rhs){
    if(lhs.number != rhs.number){
        return true;
    }
    return false;
}
bool operator<(const BigNum& lhs, const BigNum& rhs){
    if((lhs.number[0]=='+' and rhs.number[0]=='+') or (lhs.number[0]=='-' and rhs.number[0]=='-')){
        bool result = lhs.compare(lhs.number,rhs.number);
        return result;
    }
    else if(lhs.number[0]=='-' and rhs.number[0]=='+'){
        return true;
    }
    else{
        return false;
    }
}
bool operator>(const BigNum& lhs, const BigNum& rhs){
    if((lhs.number[0]=='+' and rhs.number[0]=='+') or (lhs.number[0]=='-' and rhs.number[0]=='-')){
        return !lhs.compare(lhs.number,rhs.number);
    }
    else if(lhs.number[0]=='-' and rhs.number[0]=='+'){
        return false;
    }
    else{
        return true;
    }
}
bool operator<=(const BigNum& lhs, const BigNum& rhs){
    if(lhs<rhs or lhs==rhs){
        return true;
    }
    return false;
}
bool operator>=(const BigNum& lhs, const BigNum& rhs){
    if(lhs>rhs or lhs==rhs){
        return true;
    }
    return false;
}


std::ostream& operator<<(std::ostream& lhs, const BigNum& rhs){
    for (char digit: rhs.number) {
        if(digit != '+') {
            lhs << digit;
        }
    }
    return lhs;
}

#if SUPPORT_IFSTREAM == 1
std::istream& operator>>(std::istream& lhs, BigNum& rhs){};
#endif
