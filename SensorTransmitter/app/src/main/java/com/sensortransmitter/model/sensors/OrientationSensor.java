package com.sensortransmitter.model.sensors;

import android.app.Activity;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;

import com.sensortransmitter.R;

import java.util.ArrayList;
import java.util.List;


public class OrientationSensor implements Sensor {

    private class Orientation implements Sensor.Data{
        final float yaw, roll, pitch;

        Orientation(float yaw, float roll, float pitch) {
            this.yaw = yaw;
            this.roll = roll;
            this.pitch = pitch;
        }

        public String toString() {
            return yaw + " " + roll + " " + pitch;
        }

    }

    private SensorManager sensorManager;
    private final List<SensorObserver> observers = new ArrayList<>();
    private final SensorEventListener eventListener;
    private float lastYaw = 0;

    private static final int SENSOR_DELAY = 10 * 1000; // 500ms

    public OrientationSensor(Activity activity, boolean useCompass) throws SensorException {
        sensorManager = (SensorManager) activity.getSystemService(Activity.SENSOR_SERVICE);
        android.hardware.Sensor rotationSensor = sensorManager.getDefaultSensor(android.hardware.Sensor.TYPE_ACCELEROMETER);
        android.hardware.Sensor compassSensor = sensorManager.getDefaultSensor(android.hardware.Sensor.TYPE_MAGNETIC_FIELD);

        eventListener = new SensorEventListener() {
            @Override
            public void onSensorChanged(SensorEvent event) {
                onSensorEvent(event);
            }

            @Override
            public void onAccuracyChanged(android.hardware.Sensor sensor, int accuracy) { }

        };

        if(rotationSensor == null) {
            throw new SensorException(
                    activity.getResources().getString(R.string.orientation_sensor),
                    activity.getResources().getString(R.string.accelerometer_not_found));
        }else {
            sensorManager.registerListener(eventListener, rotationSensor, SENSOR_DELAY);
            if(compassSensor != null && useCompass)
                sensorManager.registerListener(eventListener, compassSensor, SENSOR_DELAY);
        }
    }

    private float clamp(float value, float min, float max) {
        if(value > max) return max;
        if(value < min) return min;
        return value;
    }


    private void onSensorEvent(SensorEvent event) {

        if(event.sensor.getType() == android.hardware.Sensor.TYPE_ACCELEROMETER) {
            final float gravity = 9.81f;

            float yaw = lastYaw;

            float valuesClamp[] = {
                clamp(event.values[0]/gravity, -1, 1),
                clamp(event.values[1]/gravity, -1, 1),
                clamp(event.values[2]/gravity, -1, 1)
            };

            float roll  = (float)Math.atan2(valuesClamp[1], valuesClamp[0]);

            float pitch = (float)-Math.atan2(valuesClamp[2], valuesClamp[0]);

            notifyObservers(new Orientation(yaw, roll, pitch));

        }else if(event.sensor.getType() == android.hardware.Sensor.TYPE_MAGNETIC_FIELD){
            lastYaw = (float)Math.atan2(event.values[0], event.values[2]);
        }

    }

    private void notifyObservers(final Orientation orientation){
        observers.forEach(observer -> observer.onSensorEvent(orientation));
    }

    @Override
    public void subscribe(SensorObserver observer) {
        observers.add(observer);
    }

    @Override
    public void unSubscribe(SensorObserver observer) {
        observers.remove(observer);
    }

    @Override
    public void stop() {
        sensorManager.unregisterListener(eventListener);
    }
}
