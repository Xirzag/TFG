package com.sensortransmitter.view;

public interface TransmissionConfigurationView {

    void startTransmission();
    void stopTransmission();
    void showMessage(String message);
    void showMessage(int message);
    void showMessage(int message, String other);

}
