#include "mbed.h"
#include <Terminal.h>

Timeout timer;
Terminal terminal(SERIAL_TX, SERIAL_RX);
DigitalOut my_led(D3);
DigitalOut my_pwm(PA_15);

int dailyGoal = 10;
int achievedGoals = 0;

void printMenuActions(int startup)
{
    terminal.printf("\nWelcome to your own personal focus trainer. Press Enter to proceed to the Menu!\n\n");

    while(1) {
        if(startup == 0)
            terminal.getc();
        int c = terminal.getc();
        if(c == 13 || c == 10) {
            terminal.printf("1. Start a regular session.\n");
            terminal.printf("2. Set a daily goal.\n");
            terminal.printf("3. Check your progress.\n");
            terminal.printf("4. Start a five minute break.\n");
            terminal.printf("5. Start a ten minute break.\n");
            terminal.printf("6. Close the Pomodoro clock!\n");
            break;
        }
    }
}

int getMenuAction()
{
    int menu, c;
    while (terminal.scanf ("%d", &menu) != 1) {
        c = terminal.getc();
        terminal.printf ("\t\nError: invalid input ('%c'), required (int type).", c);
    }

    terminal.printf("\nEntered menu action: %d", menu);

    return menu;
}

int getDailyGoal()
{
    int goal;
    int c;
    while (terminal.scanf ("%d", &goal) != 1 || goal < 0) {
        c = terminal.getc();

        if(c == '\n')
            terminal.printf ("\t\nError: invalid input ('%c'), required (positive INT TYPE).", c);
        else
            terminal.printf ("\t\nError: invalid input ('%d'), required (POSITIVE int type).", goal);
    }

    if((goal*25) > 8*60) {
        terminal.printf("\nEntered number too high (above 8 hours): %d", goal);
        return 0;
    }

    terminal.printf("\nEntered daily work intervals: %d", goal);

    return goal;
}

void startInterval(int wait_milis, int wait_step)
{
    int prevPercent = -1;

    for(int i=0; i<wait_milis; i++) {
        wait_ms(wait_step);

        int percent = (int)((float)i/wait_milis * 100);
        if(percent % 10 == 0 && percent != prevPercent) {
            switch(percent) {
                case 0:
                    terminal.printf("\t|=");
                    break;
                case 90:
                    terminal.printf("=|");
                    break;
                default:
                    terminal.printf("=");
                    break;
            }
            prevPercent = percent;
        }
    }
}

void buzz(int duration, int step, int interval, int interval_pause)
{
    
    my_pwm = 0;
    for(int i=0;i<duration;i++)
    {        
        if(i%interval == 0)
            wait_ms(interval_pause);
            
        my_pwm = i%2;
        wait_ms(step);
    }
}

void executeMenuAction(int menuAction)
{
    switch(menuAction) {
        case 1:
            terminal.printf("\n\n\tStarting a regular session:\n\n");
            int wait_milis = 25*60;
            int wait_step = 10;
            startInterval(wait_milis, wait_step);
            achievedGoals += 1;
            buzz(2000, 1, 0, 0);
            terminal.printf("\n\n\tRegular session ended!\n");
            break;

        case 2:
            terminal.printf("\n\n\tSet daily work interval:\n");
            dailyGoal = getDailyGoal();
            terminal.printf("\n\n\tDaily work interval set to: %d\n", dailyGoal);
            break;

        case 3:
            if(dailyGoal > 0)
            {
                terminal.printf("\n\n\tAchieved\n\n\t");
                for(int i=0; i<dailyGoal; i++) {
                    if(i<achievedGoals)
                        terminal.printf("*");
                    else
                        terminal.printf("-");
                }
                 terminal.printf("\n");
            }else
                terminal.printf("\n\n\tNo daily goal currently set!\n");
            

            break;

        case 4: {
            terminal.printf("\n\n\tStarting a five minute break:\n\n");
            int wait_milis = 5*60;
            int wait_step = 10;
            startInterval(wait_milis, wait_step);
            buzz(1000, 2, 0, 0);
            terminal.printf("\n\n\tFive minute break ended!\n");
        }
        break;
        case 5: {
            terminal.printf("\n\n\tStarting a ten minute break:\n\n");
            int wait_milis = 10*60;
            int wait_step = 10;
            startInterval(wait_milis, wait_step);
            buzz(1000, 2, 0, 0);
            terminal.printf("\n\n\tTen minute break ended!\n");
        }
        break;
        case 6:
            break;

        default:
            terminal.printf("\nWrong option! Choose again.");
            break;
    }
}

int main()
{
    int menu = 0;
    int startup = 1;
    do {
        printMenuActions(startup);
        startup = 0;
        menu = getMenuAction();
        executeMenuAction(menu);
    } while(menu!=6);
}
