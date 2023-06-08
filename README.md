# Gesture Recognition System


## Description
Our project is a Gesture Recognition System that leverages the Dynamic Time Warping (DTW) algorithm. The program utilizes multiple header files to manage the functionality of the system:

- `config.h`: Contains system configurations and macro definitions.
- `sensor.h`: Manages the setup and data handling of the gyroscope sensor.
- `gesture.h` and `gesture.cpp`: Handle the recording and comparison of gestures.
- `dtw.h`: Contains the implementation of the Dynamic Time Warping (DTW) algorithm, which is used for gesture recognition.

## Dynamic Time Warping (DTW) Algorithm
The DTW algorithm is used to measure the similarity between two temporal sequences, which may vary in speed. For this project, it's used to compare a recorded gesture with a stored one, allowing for comparison of sequences of different lengths and speeds. This flexibility is crucial for gesture recognition as the same gesture can be performed at different speeds and provides robustness to noise and variations in gesture performance.

## Flow Control
The main flow of the program is centered around system flags to manage the current state of the system (recording, reading, etc.) and respond to button presses. Mutexes are used to ensure safe concurrent access to shared resources, preventing race conditions. Feedback to the user is provided via LEDs indicating recording or reading states. The program is event-driven, with a main loop handling state checks and performing the appropriate action, while a separate worker thread manages button press events.

## Usage
The `main()` function sets up the system, including the gyroscope and button interrupt, and then enters a loop where it continuously checks the system flags to determine the appropriate action. The `buttonWorkerThread()` function handles button press events and updates the system flags accordingly.

Other functions like `startRecording()`, `stopRecording()`, `startReading()`, and `stopReading()` are used to update the system flags and LEDs based on the system's current state. These updates are performed within mutex locks to prevent other threads from accessing the flags at the same time, thereby preventing data inconsistencies and race conditions.

## Conclusion
This project represents a well-structured approach to a multi-threaded, event-driven system for gesture recognition using a gyroscope and the Dynamic Time Warping algorithm. This system has numerous potential applications in real-world scenarios where gesture-based input is advantageous. 
