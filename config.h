#ifndef CONFIG_H
#define CONFIG_H

// ========== STRUCTURES ==========

// User Structure
struct Users {
    int u_id;
    char name[100];
    char email[100];
    char password[100];
    char phone[20];
    char address[200];
    char type[20];      // "Admin" or "User"
    char status[20];    // "Pending", "Active", "Inactive"
};

// Room Structure (Masterlist/Main Entity)
struct Rooms {
    int r_id;
    char room_number[10];
    char room_type[50];     // "Single", "Double", "Suite", "Deluxe"
    char description[500];
    float price_per_night;
    int capacity;           // Number of guests
    char amenities[500];
    char status[20];        // "Available", "Occupied", "Maintenance"
};

// Booking Structure
struct Bookings {
    int b_id;
    int user_id;
    int room_id;
    char check_in_date[20];
    char check_out_date[20];
    int guests;
    float total_price;
    char booking_status[20]; // "Pending", "Confirmed", "Cancelled", "Completed"
    char payment_status[20];  // "Unpaid", "Paid", "Refunded"
};

// ========== FUNCTION PROTOTYPES ==========

// User Functions
int authenticate(const char *filename, char email[], char password[], struct Users *loggedInUser);
int getNextUserID(const char *filename);
void addUser(const char *filename, struct Users u);
void updateUser(const char *filename, int target_id, struct Users newData);
void deleteUser(const char *filename, int target_id);
void viewAllUsers(const char *filename);
void viewPendingUsers(const char *filename);
void approveUser(const char *filename, int target_id);
void rejectUser(const char *filename, int target_id);
void viewUserDetails(int user_id);

// Room Functions (Masterlist/Main Entity)
int getNextRoomID(const char *filename);
void addRoom(const char *filename, struct Rooms r);
void updateRoom(const char *filename, int target_id, struct Rooms newData);
void deleteRoom(const char *filename, int target_id);
void viewAllRooms(const char *filename);
void viewAvailableRooms(const char *filename);
void viewRoomsByType(const char *filename, char room_type[]);
void updateRoomStatus(const char *filename, int target_id, char new_status[]);

// Booking Functions
int getNextBookingID(const char *filename);
void addBooking(const char *filename, struct Bookings b);
void updateBooking(const char *filename, int target_id, struct Bookings newData);
void cancelBooking(const char *filename, int target_id);
void viewUserBookings(const char *filename, int user_id);
void viewAllBookings(const char *filename);
void confirmBooking(const char *filename, int target_id);
void completeBooking(const char *filename, int target_id);
float calculateTotalPrice(float price_per_night, char check_in[], char check_out[]);

// Menu Functions
void admin_menu(struct Users loggedInUser);
void user_menu(struct Users loggedInUser);

#endif
