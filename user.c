#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#define USER_FILE "users.csv"
#define ROOM_FILE "rooms.csv"
#define BOOKING_FILE "bookings.csv"

void user_menu(struct Users loggedInUser) {
    int choice;

    while(1) {
        system("cls");
        printf("+----------------------------------------------------------+\n");
        printf("¦                 USER DASHBOARD                            ¦\n");
        printf("¦----------------------------------------------------------¦\n");
        printf("¦ Welcome, %-45s ¦\n", loggedInUser.name);
        printf("+----------------------------------------------------------+\n\n");
        
        printf("1. ?? VIEW AVAILABLE ROOMS\n");
        printf("2. ?? SEARCH ROOMS BY TYPE\n");
        printf("3. ?? MAKE A BOOKING\n");
        printf("4. ?? VIEW MY BOOKINGS\n");
        printf("5. ? CANCEL MY BOOKING\n");
        printf("6. ?? MY ACCOUNT SETTINGS\n");
        printf("7. ?? LOGOUT\n");
        printf("\nChoice: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            continue;
        }

        switch(choice) {
            case 1:
                viewAvailableRooms(ROOM_FILE);
                system("pause");
                break;

            case 2: {
                char roomType[50];
                printf("\nEnter Room Type (Single/Double/Suite/Deluxe): ");
                scanf("%s", roomType);
                viewRoomsByType(ROOM_FILE, roomType);
                system("pause");
                break;
            }

            case 3:
                make_booking(loggedInUser);
                break;

            case 4:
                viewUserBookings(BOOKING_FILE, loggedInUser.u_id);
                system("pause");
                break;

            case 5:
                cancel_my_booking(loggedInUser);
                break;

            case 6:
                user_account_settings(loggedInUser);
                break;

            case 7:
                printf("\nLogging out...\n");
                system("pause");
                return;

            default:
                printf("\nInvalid choice!\n");
                system("pause");
        }
    }
}

void make_booking(struct Users loggedInUser) {
    struct Bookings newBooking;
    struct Rooms room;
    int roomId;
    
    system("cls");
    printf("+----------------------------------------------------------+\n");
    printf("¦                 MAKE A BOOKING                            ¦\n");
    printf("+----------------------------------------------------------+\n\n");
    
    // Show available rooms first
    viewAvailableRooms(ROOM_FILE);
    
    printf("\nEnter Room ID to book: ");
    scanf("%d", &roomId);
    
    // Check if room exists and is available
    FILE *file = fopen(ROOM_FILE, "r");
    if (file == NULL) {
        printf("\nError: Cannot access room data.\n");
        system("pause");
        return;
    }
    
    char line[500];
    int found = 0;
    float price = 0;
    
    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);
        
        if (current_id == roomId) {
            sscanf(line, "%d,%9[^,],%49[^,],%499[^,],%f,%d,%499[^,],%19s",
                   &room.r_id, room.room_number, room.room_type, room.description,
                   &room.price_per_night, &room.capacity, room.amenities, room.status);
            
            if (strcmp(room.status, "Available") == 0) {
                found = 1;
                price = room.price_per_night;
            }
            break;
        }
    }
    fclose(file);
    
    if (!found) {
        printf("\n? Room not available or does not exist.\n");
        system("pause");
        return;
    }
    
    // Get booking details
    printf("\n--- Room Selected: %s (%s) ---\n", room.room_number, room.room_type);
    printf("Price per night: $%.2f\n", price);
    printf("Max capacity: %d guests\n", room.capacity);
    
    newBooking.user_id = loggedInUser.u_id;
    newBooking.room_id = roomId;
    
    printf("\nEnter Check-in Date (YYYY-MM-DD): ");
    scanf("%s", newBooking.check_in_date);
    
    printf("Enter Check-out Date (YYYY-MM-DD): ");
    scanf("%s", newBooking.check_out_date);
    
    printf("Number of Guests: ");
    scanf("%d", &newBooking.guests);
    
    if (newBooking.guests > room.capacity) {
        printf("\n? Number of guests exceeds room capacity (%d)!\n", room.capacity);
        system("pause");
        return;
    }
    
    // Calculate total price (simple calculation - 2 nights for demo)
    newBooking.total_price = calculateTotalPrice(price, newBooking.check_in_date, newBooking.check_out_date);
    
    strcpy(newBooking.booking_status, "Pending");
    strcpy(newBooking.payment_status, "Unpaid");
    
    // Confirm booking
    printf("\n--- BOOKING SUMMARY ---\n");
    printf("Room: %s (%s)\n", room.room_number, room.room_type);
    printf("Check-in: %s\n", newBooking.check_in_date);
    printf("Check-out: %s\n", newBooking.check_out_date);
    printf("Guests: %d\n", newBooking.guests);
    printf("Total Price: $%.2f\n", newBooking.total_price);
    printf("Status: Pending Approval\n");
    
    char confirm;
    printf("\nConfirm booking? (y/n): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        addBooking(BOOKING_FILE, newBooking);
        printf("\n? Your booking request has been submitted and is pending admin confirmation.\n");
    } else {
        printf("\nBooking cancelled.\n");
    }
    
    system("pause");
}

void cancel_my_booking(struct Users loggedInUser) {
    int bookingId;
    
    system("cls");
    printf("+----------------------------------------------------------+\n");
    printf("¦                 CANCEL MY BOOKING                         ¦\n");
    printf("+----------------------------------------------------------+\n\n");
    
    // Show user's bookings
    viewUserBookings(BOOKING_FILE, loggedInUser.u_id);
    
    printf("\nEnter Booking ID to cancel: ");
    scanf("%d", &bookingId);
    
    // Verify the booking belongs to this user
    FILE *file = fopen(BOOKING_FILE, "r");
    if (file == NULL) {
        printf("\nError: Cannot access booking data.\n");
        system("pause");
        return;
    }
    
    char line[500];
    int found = 0;
    int owner_id;
    
    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,%d", &current_id, &owner_id);
        
        if (current_id == bookingId && owner_id == loggedInUser.u_id) {
            found = 1;
            break;
        }
    }
    fclose(file);
    
    if (!found) {
        printf("\n? Booking ID not found or does not belong to you.\n");
        system("pause");
        return;
    }
    
    char confirm;
    printf("Are you sure you want to cancel this booking? (y/n): ");
    scanf(" %c", &confirm);
    
    if (confirm == 'y' || confirm == 'Y') {
        cancelBooking(BOOKING_FILE, bookingId);
        printf("\n? Your booking has been cancelled.\n");
    } else {
        printf("\nCancellation aborted.\n");
    }
    
    system("pause");
}

void user_account_settings(struct Users loggedInUser) {
    int choice;
    
    while(1) {
        system("cls");
        printf("+----------------------------------------------------------+\n");
        printf("¦                 ACCOUNT SETTINGS                          ¦\n");
        printf("+----------------------------------------------------------+\n\n");
        
        printf("=== MY PROFILE ===\n");
        printf("User ID:  %d\n", loggedInUser.u_id);
        printf("Name:     %s\n", loggedInUser.name);
        printf("Email:    %s\n", loggedInUser.email);
        printf("Phone:    %s\n", loggedInUser.phone);
        printf("Address:  %s\n", loggedInUser.address);
        printf("Status:   %s\n", loggedInUser.status);
        printf("--------------------------\n\n");
        
        printf("1. CHANGE PASSWORD\n");
        printf("2. UPDATE PROFILE\n");
        printf("0. BACK\n");
        printf("\nChoice: ");
        scanf("%d", &choice);
        
        if (choice == 0) break;
        
        switch(choice) {
            case 1: {
                struct Users updatedUser = loggedInUser;
                char newPass[100];
                char confirmPass[100];
                
                printf("\nEnter current password: ");
                char currentPass[100];
                scanf("%s", currentPass);
                
                if (strcmp(currentPass, loggedInUser.password) != 0) {
                    printf("? Incorrect current password!\n");
                    system("pause");
                    break;
                }
                
                printf("Enter new password: ");
                scanf("%s", newPass);
                printf("Confirm new password: ");
                scanf("%s", confirmPass);
                
                if(strcmp(newPass, confirmPass) == 0) {
                    strcpy(updatedUser.password, newPass);
                    updateUser(USER_FILE, loggedInUser.u_id, updatedUser);
                    printf("? Password updated successfully!\n");
                    strcpy(loggedInUser.password, newPass);
                } else {
                    printf("? New passwords don't match!\n");
                }
                system("pause");
                break;
            }
            
            case 2: {
                struct Users updatedUser = loggedInUser;
                
                while(getchar() != '\n');
                
                printf("\nEnter new name (Enter to keep '%s'): ", loggedInUser.name);
                char input[100];
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                if(strlen(input) > 0) strcpy(updatedUser.name, input);
                
                printf("Enter new phone (Enter to keep '%s'): ", loggedInUser.phone);
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                if(strlen(input) > 0) strcpy(updatedUser.phone, input);
                
                printf("Enter new address (Enter to keep current): ");
                fgets(input, sizeof(input), stdin);
                input[strcspn(input, "\n")] = 0;
                if(strlen(input) > 0) strcpy(updatedUser.address, input);
                
                updateUser(USER_FILE, loggedInUser.u_id, updatedUser);
                printf("? Profile updated successfully!\n");
                
                // Update logged in user info
                if(strlen(updatedUser.name) > 0) strcpy(loggedInUser.name, updatedUser.name);
                if(strlen(updatedUser.phone) > 0) strcpy(loggedInUser.phone, updatedUser.phone);
                if(strlen(updatedUser.address) > 0) strcpy(loggedInUser.address, updatedUser.address);
                
                system("pause");
                break;
            }
        }
    }
}
