package com.sensortransmitter.model.transmitters;

public interface Transmitter {

    void start();
    void send(String msg);
    void stop();

    boolean isUp();

}
