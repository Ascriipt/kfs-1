#include "printk.h"
#include "vga.h"
#include "string.h"

/* ==============================================================================
 * kernel/printk.c - Implementation de printk (formatage et affichage ecran)
 * ==============================================================================
 */

static const char hex_digits_lower[] = "0123456789abcdef";
static const char hex_digits_upper[] = "0123456789ABCDEF";

/**
 * Convertit un entier en chaine dans une base donnee (10, 16, etc.)
 * Gere correctement les entiers non signes et signes sans overflow.
 */
static int print_number(uint32_t val, int base, bool is_signed, bool upper)
{
    char buf[32];
    int count = 0;
    int i = 0;
    const char *digits = upper ? hex_digits_upper : hex_digits_lower;

    if (is_signed && ((int32_t)val < 0))
    {
        vga_putchar('-');
        count++;
        val = (uint32_t)(-(int32_t)val);
    }

    if (val == 0)
    {
        vga_putchar('0');
        return count + 1;
    }

    while (val > 0)
    {
        buf[i++] = digits[val % base];
        val /= base;
    }

    while (i > 0)
    {
        vga_putchar(buf[--i]);
        count++;
    }

    return count;
}

int vprintk(const char *fmt, va_list args)
{
    if (!fmt) return 0;

    int printed_chars = 0;

    for (size_t i = 0; fmt[i] != '\0'; i++)
    {
        if (fmt[i] != '%')
        {
            vga_putchar(fmt[i]);
            printed_chars++;
            continue;
        }

        i++;
        if (fmt[i] == '\0') break;

        switch (fmt[i])
        {
            case '%':
                vga_putchar('%');
                printed_chars++;
                break;

            case 'c':
            {
                char c = (char)va_arg(args, int);
                vga_putchar(c);
                printed_chars++;
                break;
            }

            case 's':
            {
                const char *str = va_arg(args, const char *);
                if (!str) {
                    str = "(null)";
                }
                while (*str) {
                    vga_putchar(*str++);
                    printed_chars++;
                }
                break;
            }

            case 'd':
            case 'i':
            {
                int32_t d = va_arg(args, int32_t);
                printed_chars += print_number((uint32_t)d, 10, true, false);
                break;
            }

            case 'u':
            {
                uint32_t u = va_arg(args, uint32_t);
                printed_chars += print_number(u, 10, false, false);
                break;
            }

            case 'x':
            {
                uint32_t x = va_arg(args, uint32_t);
                printed_chars += print_number(x, 16, false, false);
                break;
            }

            case 'X':
            {
                uint32_t x = va_arg(args, uint32_t);
                printed_chars += print_number(x, 16, false, true);
                break;
            }

            case 'p':
            {
                uint32_t p = (uint32_t)va_arg(args, void *);
                vga_puts("0x");
                printed_chars += 2;
                if (p == 0)
                {
                    vga_putchar('0');
                    printed_chars++;
                }
                else
                    printed_chars += print_number(p, 16, false, false);
                break;
            }

            default:
                vga_putchar('%');
                vga_putchar(fmt[i]);
                printed_chars += 2;
                break;
        }
    }

    return printed_chars;
}

int printk(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int ret = vprintk(fmt, args);
    va_end(args);
    return ret;
}
