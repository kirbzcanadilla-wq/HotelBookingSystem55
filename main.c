#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define ADMIN_CODE 1234
#define USER_FILE "users.csv"

// Function declarations
void admin_menu(struct Users loggedInUser);
void user_menu(struct Users loggedInUser);

int main() {
    struct Users u;
    struct Users loggedInUser;
    int choice, code;
    
    while(1) {
        system("cls");
        printf("+--------------------------------------+\n");
        printf("¦     HOTEL BOOKING SYSTEM 2026        ¦\n");
        printf("+--------------------------------------+\n\n");
        printf("1. Login\n");
        printf("2. Register New Account\n");
        printf("3. Exit\n");
        printf("\nEnter choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }
        
        switch(choice) {
            case 1: {
                printf("\n--- LOGIN ---\n");
                printf("Email: ");
                scanf("%s", u.email);
                printf("Password: ");
                scanf("%s", u.password);
        
                int authResult = authenticate(USER_FILE, u.email, u.password, &loggedInUser);
                
                if (authResult == 1) {
                    printf("\n? Login successful! Welcome %s\n", loggedInUser.name);
                    system("pause");
            
                    if (strcmp(loggedInUser.type, "Admin") == 0) {
                        admin_menu(loggedInUser);
                    } else {
                        user_menu(loggedInUser);
                    }
                } else if (authResult == 2) {
                    printf("\n? Your account is pending approval. Please wait for admin confirmation.\n");
                    system("pause");
                } else {
                    printf("\n? Invalid credentials!\n");
                    system("pause");
                }
                break;
            }

            case 2: {
                printf("\n--- REGISTER NEW ACCOUNT ---\n");
                
                // Check if registering as Admin
                printf("Are you registering as Admin? (y/n): ");
                char isAdmin;
                scanf(" %c", &isAdmin);
                
                if (isAdmin == 'y' || isAdmin == 'Y') {
                    printf("Enter Admin Registration Code: ");
                    scanf("%d", &code);
                    
                    if(code != ADMIN_CODE) {
                        printf("\n? INVALID CODE! Registration aborted.\n");
                        system("pause");
                        break;
                    }
                }
                
                while(getchar() != '\n');

                printf("Enter Full Name: ");
                fgets(u.name, sizeof(u.name), stdin);
                u.name[strcspn(u.name, "\n")] = 0;

                printf("Enter Email: ");
                scanf("%s", u.email);
                
                printf("Enter Password: ");
                scanf("%s", u.password);
                
                printf("Enter Phone Number: ");
                scanf("%s", u.phone);
                
                while(getchar() != '\n');
                printf("Enter Address: ");
                fgets(u.address, sizeof(u.address), stdin);
                u.address[strcspn(u.address, "\n")] = 0;
                
                if (isAdmin == 'y' || isAdmin == 'Y') {
                    strcpy(u.type, "Admin");
                    strcpy(u.status, "Active"); // Admin accounts are active immediately
                    printf("\n? Admin account created successfully! You can login now.\n");
                } else {
                    strcpy(u.type, "User");
                    strcpy(u.status, "Pending"); // User accounts need approval
                    printf("\n? Account created! Please wait for admin approval before logging in.\n");
                }
                
                addUser(USER_FILE, u);
                system("pause");               
                break;
            }

            case 3:
                printf("\nThank you for using Hotel Booking System. Goodbye!\n");
                exit(0);
                
            default:
                printf("\nInvalid choice. Try again.\n");
                system("pause");
        }
    }
    return 0;
}
