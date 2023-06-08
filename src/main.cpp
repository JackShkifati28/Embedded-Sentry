

#include "config.h"  // Include all the macro definitions and configurations.
#include "sensor.h"  // Include code for gyroscope setup and data handling.
#include "gesture.h" // Include code for gesture recording and comparison.
#include "dtw.h"     // Include code for Dynamic Time Warping.
#include "rtos/EventFlags.h"  // Include the mbed OS library for event flag handling.

// Mutex objects for managing access to the system_flags object from different threads.
Mutex recMutex;
Mutex readMutex;
Mutex stopMutex;
Mutex completeMutex;
Mutex updateMutex;

// EventFlags object to handle button press events.
rtos::EventFlags buttonPressFlag;

/* Status LED outputs: */
DigitalOut greenLed(LED1); // Define a digital output for the green LED.
DigitalOut redLed(LED2);   // Define a digital output for the red LED.

// Button input
InterruptIn button(USER_BUTTON);   // Define an interrupt input for the button.

// Create an instance of the SystemFlags struct, declared as volatile because it can be modified by an ISR.
volatile SystemFlags system_flags; 



// Function to start recording the gesture.
void startRecording() {    
     
     // Lock the mutex to prevent other threads from accessing system_flags while we're using it.
     recMutex.lock();

    // Set the flags to indicate that the system is in a recording state.
    system_flags.isRecording = 0;
    system_flags.isReading=0;
    system_flags.stop=0;
    system_flags.complete=0;
    system_flags.record=1;
    system_flags.compare=0;

    greenLed = 1; // Turn on green LED to indicate recording.
    redLed = 0;  // Unlock the mutex so that other threads can access system_flags.
    
     recMutex.unlock();
}

// Function to stop recording the gesture.
void stopRecording()
{

  // Lock the mutex to prevent other threads from accessing system_flags while we're using it.
    stopMutex.lock(); 

  // Set the flags to indicate that the system is in a stop recording state.
    system_flags.isRecording = 0;
    system_flags.isReading= 1;
    system_flags.stop= 0;
    system_flags.complete= 0;
    system_flags.record= 0;
    system_flags.compare=0;

    greenLed = 0; // Turn off green LED.
    redLed = 0;   // Turn off red LED.
      
    // Unlock the mutex so that other threads can access system_flags.
     stopMutex.unlock();
}

// Function starts when you want to start recording the compare gesture.
void startReading()
{
   readMutex.lock();

 // Set the flags to indicate that the system is in a reading state.
    system_flags.isRecording = 0;
    system_flags.isReading=0;
    system_flags.stop=1;
    system_flags.complete=0;
    system_flags.record=0;
    system_flags.compare=1;

    greenLed = 0; // Turn off green LED
    redLed = 1;   // Turn on red LED to indicate reading.
    readMutex.unlock();
}

// Function to stop reading the compare gesture.
void stopReading()
{
     completeMutex.lock();

    system_flags.isRecording = 1;
    system_flags.isReading=0;
    system_flags.stop=0;
    system_flags.complete=1;
    system_flags.record=0;

    greenLed = 0; // Turn off green LED
    redLed = 0;   // Turn off red LED
     completeMutex.unlock();
}

// Function to handle the button pressed interrupt.
void buttonPressedISR()
{
    // Set a flag to signal that the button has been pressed
    buttonPressFlag.set(1);
}

// Thread function to handle button presses.
void buttonWorkerThread()
{

    while (true)
    {
        
        // Wait for the button press flag to be set
        buttonPressFlag.wait_any(1);
         

        updateMutex.lock();

         // Reset the button press flag
         buttonPressFlag.clear(1);

        if (button.read())
        {
             // Check the current system state and take the appropriate action.
            if (system_flags.isRecording && !system_flags.isReading && !system_flags.stop)
                startRecording(); // Start recording hand gesture

            else if (!system_flags.isRecording && !system_flags.isReading && !system_flags.stop)
                stopRecording(); // done recording hand gesture

            else if (!system_flags.isRecording && system_flags.isReading && !system_flags.stop)
                startReading(); //  read hand gesture

            else if (!system_flags.isRecording && !system_flags.isReading && system_flags.stop)
                stopReading(); //  done reading hand gesture
        }
        updateMutex.unlock();
    }
}


int main()
{
    system_flags.isRecording = 1; // Set the initial system state to be recording.

    setupGyro(); // Set up the gyroscope.

    // Set up the ISR for the button press event.
    button.rise(&buttonPressedISR);


     // Start the worker thread to handle button presses.
    Thread workerThread;
    workerThread.start(buttonWorkerThread);


    while (1) {

        // Check the current system state and take the appropriate action.
         
         if (system_flags.record)  recordGesture();        // Record a hand gesture.
         
         else if (system_flags.compare) compareGesture();  // Compare the recorded gesture with the stored gesture.
         
         else if (system_flags.complete){
           
         // Check if the recorded gesture is similar to the stored gesture.
            bool isGesture = isSimilar(); 
            
            if (isGesture) printf("Gesture detected\n");
        
            else printf("Gesture Not detected\n");
        
            system_flags.complete = 0;   // Reset the complete flag.
        }
    }
}
