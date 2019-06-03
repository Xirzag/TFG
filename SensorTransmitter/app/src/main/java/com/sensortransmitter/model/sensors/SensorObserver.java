package com.sensortransmitter.model.sensors;

public interface SensorObserver {

    void onSensorEvent(Sensor.Data data);

}
