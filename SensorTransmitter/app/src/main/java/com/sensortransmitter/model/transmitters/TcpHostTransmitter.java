package com.sensortransmitter.model.transmitters;

import android.app.Activity;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;
import java.util.ListIterator;
import java.util.Locale;

import static android.content.Context.WIFI_SERVICE;

public class TcpHostTransmitter implements Transmitter{

    private final int port;
    private final List<Socket> clients;
    private ServerSocket server;
    private final ConnectionObserver observer;
    private final AsyncTask<Void, String, Void> acceptThread;

    public TcpHostTransmitter(int port, ConnectionObserver observer) {
        this.port = port;
        this.observer = observer;

        clients = new ArrayList<>();
        acceptThread = new AsyncTask<Void, String, Void>() {
            @Override
            protected Void doInBackground(Void... voids) {
                try {
                    server = new ServerSocket( port );
                    while ( isUp() )
                        addClient(server.accept());

                } catch (Exception e) {
                    publishProgress("Exception", e.getMessage());
                    stop();
                }
                return null;
            }

            private void addClient(Socket client) throws IOException {
                synchronized (clients) {
                    clients.add(client);
                }
                client.shutdownInput();
                publishProgress("NewConection", client.getInetAddress().toString());
            }

            protected void onProgressUpdate(String... values) {
                switch (values[0]) {
                    case "NewConection":
                        observer.onNewConection(values[1]);
                        break;
                    case "Exception":
                        observer.onError(values[1]);
                        break;
                }
            }

        };

    }



    @Override
    public void start() {
        acceptThread.execute();
    }

    @Override
    public void send(final String message) {

        new Thread( () ->{
                synchronized (clients) {
                    ListIterator<Socket> iterator = clients.listIterator();

                    while (iterator.hasNext())
                        sendToSocket(message, iterator.next(), iterator);

                }
        }).start();
    }

    private void sendToSocket(String message, Socket client, ListIterator<Socket> iterator) {
        try {
            if (client.isConnected() && !client.isOutputShutdown() && client.isBound()) {
                client.getOutputStream().write(message.getBytes());
            } else {
                observer.onCloseConection(client.getInetAddress().toString());
                iterator.remove();
            }
        } catch (Exception e) {
            iterator.remove();
        }
    }

    @Override
    public void stop() {
        clients.stream().
                filter(Socket::isConnected).
                forEach(client -> {
            try {
                client.close();
            } catch (IOException e) { }
        });

        clients.clear();
        try {
            server.close();
            acceptThread.cancel(true);
        } catch (Exception e) { }


    }

    @Override
    public boolean isUp() {
        return server != null && server.isBound() && !server.isClosed();
    }

    public static String getIp(Activity activity) {
        try {
            WifiManager wifiManager = (WifiManager) activity.getApplicationContext().getSystemService(WIFI_SERVICE);
            WifiInfo wifiInfo = wifiManager.getConnectionInfo();
            int ipAddress = wifiInfo.getIpAddress();
            return String.format(Locale.getDefault(), "%d.%d.%d.%d",
                    (ipAddress & 0xff), (ipAddress >> 8 & 0xff),
                    (ipAddress >> 16 & 0xff), (ipAddress >> 24 & 0xff));
        } catch (Exception ex) {
            return null;
        }
    }

}