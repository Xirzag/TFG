package com.sensortransmitter.presenter;

import com.sensortransmitter.R;
import com.sensortransmitter.model.sensors.OrientationSensor;
import com.sensortransmitter.model.sensors.Sensor;
import com.sensortransmitter.model.transmitters.ConnectionObserver;
import com.sensortransmitter.model.transmitters.TcpHostTransmitter;
import com.sensortransmitter.model.transmitters.Transmitter;
import com.sensortransmitter.view.MainActivity;
import com.sensortransmitter.view.TransmissionConfigurationView;

public class TransmissionPresenter {

    private Transmitter transmitter;
    private Sensor sensor;
    private TransmissionConfigurationView view;

    public TransmissionPresenter(TransmissionConfigurationView view) {
        this.view = view;
    }

    public void startTransmision(int port, boolean useCompass) {
        try {
            sensor = new OrientationSensor((MainActivity)view, useCompass);
            transmitter = new TcpHostTransmitter(port, new ConnectionObserver() {
                @Override
                public void onNewConection(String ip) {
                    view.showMessage(R.string.conected, ip);
                }

                @Override
                public void onCloseConection(String ip) {
                    view.showMessage(R.string.disconected, ip);
                }

                @Override
                public void onError(String errorMessage) {
                    view.showMessage(R.string.error_in, errorMessage);
                }
            });

            sensor.subscribe(data -> transmitter.send(data.toString() + '\n'));

            transmitter.start();
            view.startTransmission();

        } catch (Sensor.SensorException e) {
            view.showMessage(R.string.error_in, e.getMessage());
        }

    }

    public void stopTransmision(){
        if(transmitter != null)
            transmitter.stop();
        if(sensor != null) {
            sensor.stop();
            sensor = null;
        }
        view.stopTransmission();
    }


    public void toggleTransmision(int port, boolean useCompass){
        if(transmitter != null && transmitter.isUp()) stopTransmision();
        else startTransmision(port, useCompass);
    }

}
