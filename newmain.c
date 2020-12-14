/* 
 * Файл: main.c
 * Программа: Псевдо-случайный генератор импульсов 
 * Версия 2.0
 * Автор: Рябов В.Д.
 * Группа: 6462-110301D
 * Создан: 01.12.2017 
 * Описание: программа предназначена для генерации случайных сигналов 
 * длительностью в 30 микросекунд при случайной задержке от 100 мкс до 400 мс.
 */

/*
 * Программа предназначена для генерации случайных сигналов длительностью в
 * 30 микросекунд
 */

// Подключение нужных библиотек
#include <stdio.h>              // Стандартная библиотека ввода-вывода
#include <stdlib.h>             // Стандартная библиотека контроля
#include <xc.h>                 // Специализированная библиотека XC8

// Параметры микроконтроллера
#define _XTAL_FREQ 16000000     // Тактовая частота 4 МГц
#define BOUNCE 15               // Время в мс для обработки дребезга
#define CORRMIN 18              // Число итераций в цикле для задержки в 100 мкс
#define CORRMAX 79998           // Число итераций в цикле для задержки в 400000 мкс
// CORR - Correction factor

// Параметры сигнала
#define SIGNAL 30               // Длительность сигнала в мкс

// Обознаение выводов и кнопок
#define START RA2       // Кнопка ПУСК МК
#define STOP RA3        // Кнопка СТОП МК
#define LED RA0         // Вывод на светодиоды
#define GREEN 1         // Сигнал для зеленого светодиода
#define RED 0           // Сигнал для красного светодиода
#define GENERATOR RA1   // Выход генератора

// PIC10F322 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config FOSC = INTOSC    // Выбор генератора (внешний/внутренний)
#pragma config BOREN = ON       // Сброс при потере напряжения питания
#pragma config WDTE = OFF       // Включение/выключение сторожевого таймера
#pragma config PWRTE = OFF      // Бит разрешения работы таймера включения (PWRTE)
#pragma config MCLRE = OFF      // Перезагрузка устройства через MCLR 
#pragma config CP = ON          // Бит защиты 
#pragma config LVP = OFF        // Низковольтное программирование 
#pragma config LPBOR = ON       // Перезагрузка процессора при низкой мощности
#pragma config BORV = LO        // Выбор точки отключения (низкая/высокая)
#pragma config WRT = OFF        // Защита флеш памяти от записи

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// Определение функций
void Prepare();     // Функция подготовки МК
void CheckButton(); // Функция опроса кнопок
void RandomDelay(); // Функция генерирования случайной задержки

// Глобальные переменные
int g_bStat = 0; // Переменная для запоминания режима МК (вкл/выкл)
/* 
 * g_Random[5] хранит следующие значения:
 * 0 - Начальное значение конгруэнтный метода
 * 1 - Текунщее значение конгруэнтный метода
 * 2 - Коэффициент 'a' конгруэнтный метода
 * 3 - Коэффициент 'b' конгруэнтный медода
 */
int g_Random[4]; 


int main() // Главная функция
{
    // Объявление параметров конгруэнтного метода
    g_Random[0] = 1;
    g_Random[2] = 3;
    g_Random[3] = 5;
    Prepare();                  // Запуск функции подготовки      
    while(1)                    // Бесконечный цикл работы
    {
        CheckButton();          // Опрос кнопок
        if(g_bStat)             // Проверка режима
        {
            LED = GREEN;        // Включение зеленого светодиода (вкл МК)
            RandomDelay();      // Запуск функции случайной задержки
            GENERATOR = 1;      // Выставляем лог. 1 на выход генератора
            __delay_us(SIGNAL); // Задерживаем лог. 1 на выходе
            GENERATOR = 0;      // Обнуляем выход генератора
        }
        else
        {
            LED = RED;          // Включение красного светодиода (выкл МК)
        }
    }
    return 0;    
}

// Функция подготовки
void Prepare()
{
    TRISA = 0b1100;                         // Направление работы ножек порта А (вход)
    PORTA = 0b0000;                         // Очистка порта A
    OSCCON = (1<<4)|(1<<5)|(1<<6);          // Установка IRCF = '111' для частоты в 16 MHz
    WPUA&=~((1<<0)|(1<<1)|(1<<2)|(1<<3));   // Отключение WEAK PULL-UP
    ANSELA = 0;                             // Отключение аналоговой части
}

// Функция опроса клавиш
void CheckButton()
{
    if(!START && STOP && !g_bStat)          // Проверка на нажатие клавиши старт
    {
        __delay_ms(BOUNCE);                 // Обработка дребезга
        if(!START && STOP)                  // Обработка дребезга
        {
            g_bStat = 1;                    // Установка режима МК (вкл)            
        }
        else 
        {
            g_bStat = 0;                    // Установка режима МК (выкл)
        }
    }
    else if(START && !STOP && g_bStat)      // Проверка на нажатие клавиши стоп
    {
        __delay_ms(BOUNCE);                 // Обработка дребезга
        if(START && !STOP)                  // Обработка дребезга
        {
            g_bStat = 0;                    // Установка режима МК (выкл)
        }
    }
    else if(!START && !STOP && g_bStat)     // Защита от дурака
    {
        __delay_ms(BOUNCE);                 // Обработка дребезга
        if(!START && !STOP)                 // Обработка дребезга
        {
            g_bStat = 0;                    // Установка режима МК (выкл)
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
    
    /* Конгруэнтный метод - простой спобо для генерации случайных чисел,
     * имеет следующий алгоритм:
     * Ki = (Ki-1*a + b) mod c, имеет максимальный период = c.
     */
    g_Random[1] = (g_Random[2]*g_Random[1] + g_Random[3])%CORRMAX; 
    if(g_Random[1] < CORRMIN)               // Задержка долна быть большее 100 мкс;
    {
        g_Random[1] = (g_Random[2]*g_Random[1] + g_Random[3])%CORRMAX;
    }
    for(int i = 0; i <= g_Random[1]; i++)   // Цикл задержек в 1 мкс
    {
        __delay_us(1);
    }       
}