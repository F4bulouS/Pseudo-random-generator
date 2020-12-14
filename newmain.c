/* 
 * ����: main.c
 * ���������: ������-��������� ��������� ��������� 
 * ������ 2.0
 * �����: ����� �.�.
 * ������: 6462-110301D
 * ������: 01.12.2017 
 * ��������: ��������� ������������� ��� ��������� ��������� �������� 
 * ������������� � 30 ����������� ��� ��������� �������� �� 100 ��� �� 400 ��.
 */

/*
 * ��������� ������������� ��� ��������� ��������� �������� ������������� �
 * 30 �����������
 */

// ����������� ������ ���������
#include <stdio.h>              // ����������� ���������� �����-������
#include <stdlib.h>             // ����������� ���������� ��������
#include <xc.h>                 // ������������������ ���������� XC8

// ��������� ����������������
#define _XTAL_FREQ 16000000     // �������� ������� 4 ���
#define BOUNCE 15               // ����� � �� ��� ��������� ��������
#define CORRMIN 18              // ����� �������� � ����� ��� �������� � 100 ���
#define CORRMAX 79998           // ����� �������� � ����� ��� �������� � 400000 ���
// CORR - Correction factor

// ��������� �������
#define SIGNAL 30               // ������������ ������� � ���

// ���������� ������� � ������
#define START RA2       // ������ ���� ��
#define STOP RA3        // ������ ���� ��
#define LED RA0         // ����� �� ����������
#define GREEN 1         // ������ ��� �������� ����������
#define RED 0           // ������ ��� �������� ����������
#define GENERATOR RA1   // ����� ����������

// PIC10F322 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTOSC    // ����� ���������� (�������/����������)
#pragma config BOREN = ON       // ����� ��� ������ ���������� �������
#pragma config WDTE = OFF       // ���������/���������� ����������� �������
#pragma config PWRTE = OFF      // ��� ���������� ������ ������� ��������� (PWRTE)
#pragma config MCLRE = OFF      // ������������ ���������� ����� MCLR 
#pragma config CP = ON          // ��� ������ 
#pragma config LVP = OFF        // ������������� ���������������� 
#pragma config LPBOR = ON       // ������������ ���������� ��� ������ ��������
#pragma config BORV = LO        // ����� ����� ���������� (������/�������)
#pragma config WRT = OFF        // ������ ���� ������ �� ������

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// ����������� �������
void Prepare();     // ������� ���������� ��
void CheckButton(); // ������� ������ ������
void RandomDelay(); // ������� ������������� ��������� ��������

// ���������� ����������
int g_bStat = 0; // ���������� ��� ����������� ������ �� (���/����)
/* 
 * g_Random[5] ������ ��������� ��������:
 * 0 - ��������� �������� ������������ ������
 * 1 - �������� �������� ������������ ������
 * 2 - ����������� 'a' ������������ ������
 * 3 - ����������� 'b' ������������ ������
 */
int g_Random[4]; 


int main() // ������� �������
{
    // ���������� ���������� ������������� ������
    g_Random[0] = 1;
    g_Random[2] = 3;
    g_Random[3] = 5;
    Prepare();                  // ������ ������� ����������      
    while(1)                    // ����������� ���� ������
    {
        CheckButton();          // ����� ������
        if(g_bStat)             // �������� ������
        {
            LED = GREEN;        // ��������� �������� ���������� (��� ��)
            RandomDelay();      // ������ ������� ��������� ��������
            GENERATOR = 1;      // ���������� ���. 1 �� ����� ����������
            __delay_us(SIGNAL); // ����������� ���. 1 �� ������
            GENERATOR = 0;      // �������� ����� ����������
        }
        else
        {
            LED = RED;          // ��������� �������� ���������� (���� ��)
        }
    }
    return 0;    
}

// ������� ����������
void Prepare()
{
    TRISA = 0b1100;                         // ����������� ������ ����� ����� � (����)
    PORTA = 0b0000;                         // ������� ����� A
    OSCCON = (1<<4)|(1<<5)|(1<<6);          // ��������� IRCF = '111' ��� ������� � 16 MHz
    WPUA&=~((1<<0)|(1<<1)|(1<<2)|(1<<3));   // ���������� WEAK PULL-UP
    ANSELA = 0;                             // ���������� ���������� �����
}

// ������� ������ ������
void CheckButton()
{
    if(!START && STOP && !g_bStat)          // �������� �� ������� ������� �����
    {
        __delay_ms(BOUNCE);                 // ��������� ��������
        if(!START && STOP)                  // ��������� ��������
        {
            g_bStat = 1;                    // ��������� ������ �� (���)            
        }
        else 
        {
            g_bStat = 0;                    // ��������� ������ �� (����)
        }
    }
    else if(START && !STOP && g_bStat)      // �������� �� ������� ������� ����
    {
        __delay_ms(BOUNCE);                 // ��������� ��������
        if(START && !STOP)                  // ��������� ��������
        {
            g_bStat = 0;                    // ��������� ������ �� (����)
        }
    }
    else if(!START && !STOP && g_bStat)     // ������ �� ������
    {
        __delay_ms(BOUNCE);                 // ��������� ��������
        if(!START && !STOP)                 // ��������� ��������
        {
            g_bStat = 0;                    // ��������� ������ �� (����)
        }
    }
}

void RandomDelay()
{
    /*if(g_Random[2] % 2 == 0)
    {
        g_Random[1] = (c*g_Random[1] + d)%DELMAX;
        g_Random[2]++;
        if(g_Random[1] < 100)
        {
            g_Random[1] = (c*g_Random[1] + d)%DELMAX;
        }
        for(int i = 0; i <= g_Random[1] - 100; i++)
        {
            __delay_us(1);
        }             
    }
    else
    {
        g_Random[1] = (a*g_Random[1] + b)%DELMAX;
        g_Random[2]++;
        if(g_Random[1] < 100)
        {
            g_Random[1] = (c*g_Random[1] + d)%DELMAX;
        }
        for(int i = 0; i <= g_Random[1] - 100; i++)
        {
            __delay_us(1);
        }
    }*/ 
    
    /* ������������ ����� - ������� ����� ��� ��������� ��������� �����,
     * ����� ��������� ��������:
     * Ki = (Ki-1*a + b) mod c, ����� ������������ ������ = c.
     */
    g_Random[1] = (g_Random[2]*g_Random[1] + g_Random[3])%CORRMAX; 
    if(g_Random[1] < CORRMIN)               // �������� ����� ���� ������� 100 ���;
    {
        g_Random[1] = (g_Random[2]*g_Random[1] + g_Random[3])%CORRMAX;
    }
    for(int i = 0; i <= g_Random[1]; i++)   // ���� �������� � 1 ���
    {
        __delay_us(1);
    }       
}