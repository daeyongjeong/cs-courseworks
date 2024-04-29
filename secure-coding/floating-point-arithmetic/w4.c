#include <stdio.h>
#include <memory.h>
#define EXP_BIT_SIZE 8
#define MANTISSA_BIT_SIZE 23

union ieee754_singleprecision
{
    float f_a;
    struct struct_ieee754_singleprecision_t
    {
        unsigned int mantissa : 23; // mantissa
        unsigned int exp : 8;       // exponent
        int sign : 1;               // sign
    } floatData;                    // IEEE754SinglePrecision
} floatVal;                         // float_IEEE754

void GetBitString(unsigned int value, int size, char *str_out);

int main()
{
    floatVal.f_a = 0.875f;
    char sign_bit, exp_bit_str[EXP_BIT_SIZE + 1], mantissa_bit_str[MANTISSA_BIT_SIZE + 1];
    printf("sign : %c\n", floatVal.floatData.sign == 0 ? '+' : '-');
    printf("exponent : 0X%X\n", floatVal.floatData.exp);
    printf("mantissa : 0X%X\n", floatVal.floatData.mantissa);
    sign_bit = floatVal.floatData.sign == 0 ? '0' : '1';
    GetBitString(floatVal.floatData.exp, EXP_BIT_SIZE, exp_bit_str);
    GetBitString(floatVal.floatData.mantissa, MANTISSA_BIT_SIZE, mantissa_bit_str);
    printf("%c / %s / %s\n", sign_bit, exp_bit_str, mantissa_bit_str);
    // 부호변환전
    printf("\nBefore changed sign bit :%f\n", floatVal.f_a);
    // 부호변환후
    floatVal.floatData.sign = 1;
    printf("sign : %c\n", floatVal.floatData.sign == 0 ? '+' : '-');
    printf("exponent : 0X%X\n", floatVal.floatData.exp);
    printf("mantissa : 0X%X\n", floatVal.floatData.mantissa);
    sign_bit = floatVal.floatData.sign == 0 ? '0' : '1';
    GetBitString(floatVal.floatData.exp, EXP_BIT_SIZE, exp_bit_str);
    GetBitString(floatVal.floatData.mantissa, MANTISSA_BIT_SIZE, mantissa_bit_str);
    printf("%c / %s / %s\n", sign_bit, exp_bit_str, mantissa_bit_str);
    printf("\nAfter changed sign bit :%f\n", floatVal.f_a);

    unsigned int bitContent;
    memcpy(&bitContent, &floatVal.f_a, sizeof(bitContent));
    GetBitString(bitContent, sizeof(bitContent) * 8, exp_bit_str);
    printf("\nBit content of floatVal.f_a: %s\n", exp_bit_str);
    
    return 0;
}

void GetBitString(unsigned int value, int size, char *str_out)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (value >> (size - i - 1) & 1)
        {
            str_out[i] = '1';
        }
        else
        {
            str_out[i] = '0';
        }
    }
    str_out[i] = '\0';
}