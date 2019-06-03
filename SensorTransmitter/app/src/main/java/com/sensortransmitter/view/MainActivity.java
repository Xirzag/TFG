package com.sensortransmitter.view;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Toast;

import com.sensortransmitter.R;
import com.sensortransmitter.model.transmitters.TcpHostTransmitter;
import com.sensortransmitter.presenter.TransmissionPresenter;

public class MainActivity extends AppCompatActivity implements TransmissionConfigurationView {

    TransmissionPresenter transmissionPresenter;
    Button toggleTransmission;
    EditText portInput;
    EditText ipInput;
    CheckBox compassInput;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        transmissionPresenter = new TransmissionPresenter(this);

        compassInput = (CheckBox)findViewById(R.id.useCompassInput);

        toggleTransmission = (Button)findViewById(R.id.startButton);
        toggleTransmission.setOnClickListener(view -> {
            int port = Integer.parseInt(((EditText)findViewById(R.id.portInput)).getText().toString());
            boolean useCompass = compassInput.isChecked();
            transmissionPresenter.toggleTransmision(port, useCompass);
        });

        portInput = (EditText)findViewById(R.id.portInput);
        ipInput = (EditText)findViewById(R.id.ipInput);

        ipInput.setText(TcpHostTransmitter.getIp(this));

    }


    @Override
    public void startTransmission() {
        portInput.setEnabled(false);
        compassInput.setEnabled(false);
        toggleTransmission.setText(R.string.stop_server);
        showMessage(getResources().getString(R.string.server_started));
    }

    @Override
    public void stopTransmission() {
        portInput.setEnabled(true);
        compassInput.setEnabled(true);
        toggleTransmission.setText(R.string.start_server);
    }

    @Override
    public void showMessage(String message) {
        Toast.makeText(this, message, Toast.LENGTH_SHORT).show();
    }

    @Override
    public void showMessage(int message) {
        showMessage(getResources().getString(message));
    }

    @Override
    public void showMessage(int message, String other) {
        showMessage(getResources().getString(message) + other);
    }

    @Override
    public boolean isFinishing() {
        boolean shouldFinish = super.isFinishing();
        if(shouldFinish)
            transmissionPresenter.stopTransmision();

        return shouldFinish;
    }
}
