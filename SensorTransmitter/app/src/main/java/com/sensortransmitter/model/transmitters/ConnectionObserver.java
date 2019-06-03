package com.sensortransmitter.model.transmitters;

public interface ConnectionObserver {

    void onNewConection(String ip);
    void onCloseConection(String ip);
    void onError(String errorMessage);

}
