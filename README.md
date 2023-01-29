===>STEPS FOR COMPILING AND RUNNING THE CODE (BASIC COMMUNICATION APP)<===

These are the requirements and steps require to run the basic communication application:
Ensure that openssl dev is installed on your system.

Ensure the system is connected to the internet.

Change the terminal directory to the project folder.

Compile the code by typing or copy and paste the command below into your terminal:

gcc auth.c cJSON_Utils.c cJSON.c ftp.c main.c network.c services.c -o main -lssl -lcrypto

Then run the code with the command: ./main

In case you want to see logs than you see while running the code, there is a
macro called #define DEBUG in the network.h file, it's currently commented out, 
uncomment it to see more logs such as request packet, responses etc.

