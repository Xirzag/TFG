package com.sensortransmitter.model.sensors;

public interface Sensor {

    interface Data{
        String toString();
    }

    class SensorException extends Exception{

        final String sensor;

        SensorException(String sensorName, String message) {
            super(sensorName + ": " + message);
            this.sensor = sensorName;
        }

    }

    void subscribe(SensorObserver observer) ;
    void unSubscribe(SensorObserver observer);
    void stop();

}
