#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "config.h"

// ========== FILE NAMES ==========
#define USER_FILE "users.csv"
#define ROOM_FILE "rooms.csv"
#define BOOKING_FILE "bookings.csv"

// ========== HELPER FUNCTIONS ==========

// Calculate total price based on nights
float calculateTotalPrice(float price_per_night, char check_in[], char check_out[]) {
    // Simple calculation - in real app, calculate days between dates
    return price_per_night * 2; // Placeholder for 2 nights
}

// Get next ID for any file
int getNextID(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) return 1;

    int maxID = 0, currentID;
    char line[500];
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%d,", &currentID) == 1) {
            if (currentID > maxID) maxID = currentID;
        }
    }
    fclose(file);
    return maxID + 1;
}

// ========== USER FUNCTIONS ==========

int getNextUserID(const char *filename) {
    return getNextID(filename);
}

void addUser(const char *filename, struct Users u) {
    u.u_id = getNextUserID(filename);

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("\nError: Could not open file for writing!\n");
        return;
    }

    fprintf(file, "%d,%s,%s,%s,%s,%s,%s,%s\n", 
            u.u_id, u.name, u.email, u.password, u.phone, u.address, u.type, u.status);
    fclose(file);
    printf("\n? RECORD ADDED! Assigned ID: %d", u.u_id);
}

int authenticate(const char *filename, char email[], char password[], struct Users *loggedInUser) {
    FILE *file = fopen(filename, "r");
    struct Users u;
    char line[500];

    if (file == NULL) return 0;

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        int fields = sscanf(line, "%d,%99[^,],%99[^,],%99[^,],%19[^,],%199[^,],%19[^,],%19s",
                            &u.u_id, u.name, u.email, u.password, u.phone, u.address, u.type, u.status);

        if (fields == 8 && strcmp(u.email, email) == 0 && strcmp(u.password, password) == 0) {
            if (strcmp(u.status, "Active") == 0) {
                *loggedInUser = u;
                fclose(file);
                return 1;
            } else if (strcmp(u.status, "Pending") == 0) {
                fclose(file);
                return 2; // Pending approval
            }
        }
    }
    fclose(file);
    return 0;
}

void viewAllUsers(const char *filename) {
    FILE *file = fopen(filename, "r");
    struct Users u;
    char line[500];

    if (file == NULL) {
        printf("\nNo records found.\n");
        return;
    }

    printf("\n+------------------------------------------------------------------------------------+");
    printf("\n¦ ID    ¦ NAME               ¦ EMAIL                      ¦ TYPE         ¦ STATUS    ¦");
    printf("\n+-------+--------------------+----------------------------+--------------+-----------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        int fields = sscanf(line, "%d,%99[^,],%99[^,],%99[^,],%19[^,],%199[^,],%19[^,],%19s",
                            &u.u_id, u.name, u.email, u.password, u.phone, u.address, u.type, u.status);
        if (fields >= 8) {
            printf("\n¦ %-5d ¦ %-18s ¦ %-26s ¦ %-12s ¦ %-9s ¦", 
                   u.u_id, u.name, u.email, u.type, u.status);
        }
    }
    printf("\n+------------------------------------------------------------------------------------+\n");
    fclose(file);
}

void viewPendingUsers(const char *filename) {
    FILE *file = fopen(filename, "r");
    struct Users u;
    char line[500];
    int found = 0;

    if (file == NULL) {
        printf("\nNo records found.\n");
        return;
    }

    printf("\n+--------------------------------------------------------------------------------+");
    printf("\n¦ ID    ¦ NAME               ¦ EMAIL                      ¦ PHONE                ¦");
    printf("\n+-------+--------------------+----------------------------+----------------------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        int fields = sscanf(line, "%d,%99[^,],%99[^,],%99[^,],%19[^,],%199[^,],%19[^,],%19s",
                            &u.u_id, u.name, u.email, u.password, u.phone, u.address, u.type, u.status);
        if (fields >= 8 && strcmp(u.status, "Pending") == 0) {
            printf("\n¦ %-5d ¦ %-18s ¦ %-26s ¦ %-20s ¦", 
                   u.u_id, u.name, u.email, u.phone);
            found = 1;
        }
    }
    printf("\n+--------------------------------------------------------------------------------+\n");
    
    if (!found) {
        printf("\nNo pending users found.\n");
    }
    fclose(file);
}

void approveUser(const char *filename, int target_id) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            struct Users u;
            sscanf(line, "%d,%99[^,],%99[^,],%99[^,],%19[^,],%199[^,],%19[^,],%19s",
                   &u.u_id, u.name, u.email, u.password, u.phone, u.address, u.type, u.status);
            fprintf(temp, "%d,%s,%s,%s,%s,%s,%s,Active\n", 
                    u.u_id, u.name, u.email, u.password, u.phone, u.address, u.type);
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.csv", filename);

    if(found) printf("\n? User ID %d has been APPROVED!\n", target_id);
    else printf("\nUser ID %d not found.\n", target_id);
}

void rejectUser(const char *filename, int target_id) {
    // For rejection, we can either delete or mark as rejected
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            found = 1; // Skip writing this line (delete)
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.csv", filename);

    if(found) printf("\n? User ID %d has been REJECTED and removed.\n", target_id);
    else printf("\nUser ID %d not found.\n", target_id);
}

void updateUser(const char *filename, int target_id, struct Users newData) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            fprintf(temp, "%d,%s,%s,%s,%s,%s,%s,%s\n", 
                    target_id, newData.name, newData.email, newData.password,
                    newData.phone, newData.address, newData.type, newData.status);
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.csv", filename);

    if(found) printf("\n? User ID %d updated successfully!\n", target_id);
    else printf("\nUser ID %d not found.\n", target_id);
}

void deleteUser(const char *filename, int target_id) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp.csv", filename);

    if(found) printf("\n? User ID %d deleted successfully!\n", target_id);
    else printf("\nUser ID %d not found.\n", target_id);
}

// ========== ROOM FUNCTIONS (Masterlist/Main Entity) ==========

int getNextRoomID(const char *filename) {
    return getNextID(filename);
}

void addRoom(const char *filename, struct Rooms r) {
    r.r_id = getNextRoomID(filename);

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("\nError: Could not open file for writing!\n");
        return;
    }

    fprintf(file, "%d,%s,%s,%s,%.2f,%d,%s,%s\n", 
            r.r_id, r.room_number, r.room_type, r.description, 
            r.price_per_night, r.capacity, r.amenities, r.status);
    fclose(file);
    printf("\n? ROOM ADDED! Room ID: %d, Room Number: %s\n", r.r_id, r.room_number);
}

void viewAllRooms(const char *filename) {
    FILE *file = fopen(filename, "r");
    struct Rooms r;
    char line[500];

    if (file == NULL) {
        printf("\nNo rooms found.\n");
        return;
    }

    printf("\n+-------------------------------------------------------------------------------+");
    printf("\n¦ ID    ¦ ROOM NO    ¦ TYPE               ¦ PRICE/NIGHT  ¦ CAPACITY ¦ STATUS    ¦");
    printf("\n+-------+------------+--------------------+--------------+----------+-----------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, "%d,%9[^,],%49[^,],%499[^,],%f,%d,%499[^,],%19s",
                   &r.r_id, r.room_number, r.room_type, r.description, 
                   &r.price_per_night, &r.capacity, r.amenities, r.status) == 8) {
            printf("\n¦ %-5d ¦ %-10s ¦ %-18s ¦ $%-11.2f ¦ %-8d ¦ %-9s ¦", 
                   r.r_id, r.room_number, r.room_type, r.price_per_night, r.capacity, r.status);
        }
    }
    printf("\n+-------------------------------------------------------------------------------+\n");
    fclose(file);
}

void viewAvailableRooms(const char *filename) {
    FILE *file = fopen(filename, "r");
    struct Rooms r;
    char line[500];
    int found = 0;

    if (file == NULL) {
        printf("\nNo rooms found.\n");
        return;
    }

    printf("\n+-------------------------------------------------------------------+");
    printf("\n¦ ID    ¦ ROOM NO    ¦ TYPE               ¦ PRICE/NIGHT  ¦ CAPACITY ¦");
    printf("\n+-------+------------+--------------------+--------------+----------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, "%d,%9[^,],%49[^,],%499[^,],%f,%d,%499[^,],%19s",
                   &r.r_id, r.room_number, r.room_type, r.description, 
                   &r.price_per_night, &r.capacity, r.amenities, r.status) == 8) {
            if (strcmp(r.status, "Available") == 0) {
                printf("\n¦ %-5d ¦ %-10s ¦ %-18s ¦ $%-11.2f ¦ %-8d ¦", 
                       r.r_id, r.room_number, r.room_type, r.price_per_night, r.capacity);
                found = 1;
            }
        }
    }
    printf("\n+-------------------------------------------------------------------+\n");
    
    if (!found) {
        printf("\nNo available rooms at the moment.\n");
    }
    fclose(file);
}

void viewRoomsByType(const char *filename, char room_type[]) {
    FILE *file = fopen(filename, "r");
    struct Rooms r;
    char line[500];
    int found = 0;

    if (file == NULL) {
        printf("\nNo rooms found.\n");
        return;
    }

    printf("\n+----------------------------------------------------------+");
    printf("\n¦ ID    ¦ ROOM NO    ¦ PRICE/NIGHT  ¦ CAPACITY ¦ STATUS    ¦");
    printf("\n+-------+------------+--------------+----------+-----------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, "%d,%9[^,],%49[^,],%499[^,],%f,%d,%499[^,],%19s",
                   &r.r_id, r.room_number, r.room_type, r.description, 
                   &r.price_per_night, &r.capacity, r.amenities, r.status) == 8) {
            if (strcmp(r.room_type, room_type) == 0) {
                printf("\n¦ %-5d ¦ %-10s ¦ $%-11.2f ¦ %-8d ¦ %-9s ¦", 
                       r.r_id, r.room_number, r.price_per_night, r.capacity, r.status);
                found = 1;
            }
        }
    }
    printf("\n+----------------------------------------------------------+\n");
    
    if (!found) {
        printf("\nNo rooms of type '%s' found.\n", room_type);
    }
    fclose(file);
}

void updateRoom(const char *filename, int target_id, struct Rooms newData) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp_rooms.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            fprintf(temp, "%d,%s,%s,%s,%.2f,%d,%s,%s\n", 
                    target_id, newData.room_number, newData.room_type, newData.description,
                    newData.price_per_night, newData.capacity, newData.amenities, newData.status);
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_rooms.csv", filename);

    if(found) printf("\n? Room ID %d updated successfully!\n", target_id);
    else printf("\nRoom ID %d not found.\n", target_id);
}

void updateRoomStatus(const char *filename, int target_id, char new_status[]) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp_rooms.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            struct Rooms r;
            sscanf(line, "%d,%9[^,],%49[^,],%499[^,],%f,%d,%499[^,],%19s",
                   &r.r_id, r.room_number, r.room_type, r.description, 
                   &r.price_per_night, &r.capacity, r.amenities, r.status);
            fprintf(temp, "%d,%s,%s,%s,%.2f,%d,%s,%s\n", 
                    r.r_id, r.room_number, r.room_type, r.description,
                    r.price_per_night, r.capacity, r.amenities, new_status);
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_rooms.csv", filename);

    if(found) printf("\n? Room ID %d status updated to %s\n", target_id, new_status);
    else printf("\nRoom ID %d not found.\n", target_id);
}

void deleteRoom(const char *filename, int target_id) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp_rooms.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            found = 1;
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_rooms.csv", filename);

    if(found) printf("\n? Room ID %d deleted successfully!\n", target_id);
    else printf("\nRoom ID %d not found.\n", target_id);
}

// ========== BOOKING FUNCTIONS ==========

int getNextBookingID(const char *filename) {
    return getNextID(filename);
}

void addBooking(const char *filename, struct Bookings b) {
    b.b_id = getNextBookingID(filename);

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("\nError: Could not open file for writing!\n");
        return;
    }

    fprintf(file, "%d,%d,%d,%s,%s,%d,%.2f,%s,%s\n", 
            b.b_id, b.user_id, b.room_id, b.check_in_date, b.check_out_date,
            b.guests, b.total_price, b.booking_status, b.payment_status);
    fclose(file);
    printf("\n? BOOKING CONFIRMED! Booking ID: %d\n", b.b_id);
}

void viewUserBookings(const char *filename, int user_id) {
    FILE *file = fopen(filename, "r");
    struct Bookings b;
    char line[500];
    int found = 0;

    if (file == NULL) {
        printf("\nNo bookings found.\n");
        return;
    }

    printf("\n+---------------------------------------------------------------------------+");
    printf("\n¦ ID    ¦ ROOM ID  ¦ CHECK-IN     ¦ CHECK-OUT    ¦ TOTAL PRICE  ¦ STATUS    ¦");
    printf("\n+-------+----------+--------------+--------------+--------------+-----------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, "%d,%d,%d,%19[^,],%19[^,],%d,%f,%19[^,],%19s",
                   &b.b_id, &b.user_id, &b.room_id, b.check_in_date, b.check_out_date,
                   &b.guests, &b.total_price, b.booking_status, b.payment_status) == 9) {
            if (b.user_id == user_id) {
                printf("\n¦ %-5d ¦ %-8d ¦ %-12s ¦ %-12s ¦ $%-10.2f ¦ %-9s ¦", 
                       b.b_id, b.room_id, b.check_in_date, b.check_out_date, 
                       b.total_price, b.booking_status);
                found = 1;
            }
        }
    }
    printf("\n+---------------------------------------------------------------------------+\n");
    
    if (!found) {
        printf("\nNo bookings found for this user.\n");
    }
    fclose(file);
}

void viewAllBookings(const char *filename) {
    FILE *file = fopen(filename, "r");
    struct Bookings b;
    char line[500];

    if (file == NULL) {
        printf("\nNo bookings found.\n");
        return;
    }

    printf("\n+--------------------------------------------------------------------------------------------------+");
    printf("\n¦ ID    ¦ USER ID  ¦ ROOM ID  ¦ CHECK-IN     ¦ CHECK-OUT    ¦ TOTAL PRICE  ¦ STATUS    ¦ PAYMENT   ¦");
    printf("\n+-------+----------+----------+--------------+--------------+--------------+-----------+-----------¦");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = 0;
        if (sscanf(line, "%d,%d,%d,%19[^,],%19[^,],%d,%f,%19[^,],%19s",
                   &b.b_id, &b.user_id, &b.room_id, b.check_in_date, b.check_out_date,
                   &b.guests, &b.total_price, b.booking_status, b.payment_status) == 9) {
            printf("\n¦ %-5d ¦ %-8d ¦ %-8d ¦ %-12s ¦ %-12s ¦ $%-10.2f ¦ %-9s ¦ %-9s ¦", 
                   b.b_id, b.user_id, b.room_id, b.check_in_date, b.check_out_date, 
                   b.total_price, b.booking_status, b.payment_status);
        }
    }
    printf("\n+--------------------------------------------------------------------------------------------------+\n");
    fclose(file);
}

void confirmBooking(const char *filename, int target_id) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp_bookings.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            struct Bookings b;
            sscanf(line, "%d,%d,%d,%19[^,],%19[^,],%d,%f,%19[^,],%19s",
                   &b.b_id, &b.user_id, &b.room_id, b.check_in_date, b.check_out_date,
                   &b.guests, &b.total_price, b.booking_status, b.payment_status);
            fprintf(temp, "%d,%d,%d,%s,%s,%d,%.2f,%s,%s\n", 
                    b.b_id, b.user_id, b.room_id, b.check_in_date, b.check_out_date,
                    b.guests, b.total_price, "Confirmed", b.payment_status);
            found = 1;
            
            // Update room status to Occupied
            updateRoomStatus(ROOM_FILE, b.room_id, "Occupied");
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_bookings.csv", filename);

    if(found) printf("\n? Booking ID %d has been CONFIRMED!\n", target_id);
    else printf("\nBooking ID %d not found.\n", target_id);
}

void cancelBooking(const char *filename, int target_id) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp_bookings.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            struct Bookings b;
            sscanf(line, "%d,%d,%d,%19[^,],%19[^,],%d,%f,%19[^,],%19s",
                   &b.b_id, &b.user_id, &b.room_id, b.check_in_date, b.check_out_date,
                   &b.guests, &b.total_price, b.booking_status, b.payment_status);
            fprintf(temp, "%d,%d,%d,%s,%s,%d,%.2f,%s,%s\n", 
                    b.b_id, b.user_id, b.room_id, b.check_in_date, b.check_out_date,
                    b.guests, b.total_price, "Cancelled", "Refunded");
            found = 1;
            
            // Update room status back to Available
            updateRoomStatus(ROOM_FILE, b.room_id, "Available");
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_bookings.csv", filename);

    if(found) printf("\n? Booking ID %d has been CANCELLED!\n", target_id);
    else printf("\nBooking ID %d not found.\n", target_id);
}

void completeBooking(const char *filename, int target_id) {
    FILE *file = fopen(filename, "r");
    FILE *temp = fopen("temp_bookings.csv", "w");
    char line[500];
    int found = 0;

    if (!file || !temp) {
        printf("\nError: File access failed!\n");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        int current_id;
        sscanf(line, "%d,", &current_id);

        if (current_id == target_id) {
            struct Bookings b;
            sscanf(line, "%d,%d,%d,%19[^,],%19[^,],%d,%f,%19[^,],%19s",
                   &b.b_id, &b.user_id, &b.room_id, b.check_in_date, b.check_out_date,
                   &b.guests, &b.total_price, b.booking_status, b.payment_status);
            fprintf(temp, "%d,%d,%d,%s,%s,%d,%.2f,%s,%s\n", 
                    b.b_id, b.user_id, b.room_id, b.check_in_date, b.check_out_date,
                    b.guests, b.total_price, "Completed", "Paid");
            found = 1;
            
            // Update room status back to Available
            updateRoomStatus(ROOM_FILE, b.room_id, "Available");
        } else {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    remove(filename);
    rename("temp_bookings.csv", filename);

    if(found) printf("\n? Booking ID %d has been COMPLETED!\n", target_id);
    else printf("\nBooking ID %d not found.\n", target_id);
}
