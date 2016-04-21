#pragma config(Sensor, S1,     DGPS,           sensorI2CCustom)
#pragma config(Sensor, S2,     TOUCH,          sensorEV3_Touch)
#pragma config(Motor,  motorA,          STEERING,      tmotorEV3_Medium, PIDControl, encoder)
#pragma config(Motor,  motorB,          LEFT,          tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          RIGHT,         tmotorEV3_Large, PIDControl, encoder)
//#pragma config(Sensor, S4,     HTCOMPASS,      sensorI2CCustom)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "hitechnic-compass.h"
#include "dexterind-gps.h"


long steeringFactor = 45;                //Fine tune how sharp the turn is for the specific vehicle

void steerRight()
{
  if (getMotorEncoder(STEERING) > -steeringFactor) //"Turn Right" Operation
	   {
	    setMotorSpeed(STEERING, -50);
	    waitUntil(getMotorEncoder(STEERING) < -steeringFactor);
	    setMotorSpeed(STEERING, 0);
	   }
	  else
	   {
	    setMotorSpeed(STEERING, 0);
	   }
}

void steerLeft()
{
	if (getMotorEncoder(STEERING) < steeringFactor) //"Turn Left" Operation
	   {
	    setMotorSpeed(STEERING, 50);
	    waitUntil(getMotorEncoder(STEERING) > steeringFactor);
	    setMotorSpeed(STEERING, 0);
	   }
	  else
	   {
	    setMotorSpeed(STEERING, 0);
	   }
}

void steerRecenter()
{
	if (getMotorEncoder(STEERING) == 0)  // "Steer Recenter" Operation
	  	{
	  	 setMotorSpeed(STEERING, 0);
	  	}
	  else if (getMotorEncoder(STEERING) > 3)
	       {
	         setMotorSpeed(STEERING, -50);
	         waitUntil(getMotorEncoder(STEERING) < 3);
	         setMotorSpeed(STEERING, 0);
	       }
	      else if (getMotorEncoder (STEERING) < -3)
	       {
	         setMotorSpeed(STEERING, 50);
	         waitUntil(getMotorEncoder(STEERING) > -3);
	         setMotorSpeed(STEERING, 0);
	       }
}


task main()
{
  //Create and configure struct for the compass:
   // Create struct to hold sensor data:
   tHTMC compass;
   // Initialise and configure struct and port:
   initSensor(&compass, S4);

  //Define destination coordinates:
  long destLat = 0;		// Add the latitude destination here.
  long destLong = 0;	// Add the longitude destination here.

  long GPSrelHeading;
  long distance;

  DGPSsetDestination(DGPS, destLat, destLong);

  while (DGPSreadDistToDestination(DGPS) > 0) //Travelling to destination
  		{
        GPSrelHeading = DGPSreadRelHeading(DGPS);
        distance = DGPSreadDistToDestination(DGPS);

  		  // Here, the relative heading of the GPS is set as the target heading of the compass;
        // a compass.relativeHeading of 0 means the destination is straight ahead, > 0 heading means it
        // is to the left, and < 0 means the dest is to the right.
        readSensor(&compass);
        compass.offset = GPSrelHeading;

        displayTextLine(3, "GPSRelHead: %d", GPSrelHeading);
    		readSensor(&compass);
		    displayTextLine(5, "CompRelHead: %d", compass.relativeHeading);
  		  displayTextLine(7, "Distance: %d", distance);

  		  setMotorSpeed(LEFT, 75);
  			setMotorSpeed(RIGHT, 75);

  			readSensor(&compass);
  		  if (compass.relativeHeading > 5) //If car is right of dest., turn left
  			     {
  				     steerLeft();
  			     }
  			     else if (compass.relativeHeading < -5) //If car is left of dest., turn right
  			          {
  			            steerRight();
  			          }
  			          else //dest. is forward
  			          {
  			            steerRecenter();
  			          }
  		  sleep(1000);
  	  }
  		//Destination reached
  		setMotorSpeed(LEFT, 0);
  		setMotorSpeed(RIGHT, 0);
  		steerRecenter();
}