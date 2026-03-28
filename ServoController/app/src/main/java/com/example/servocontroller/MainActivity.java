package com.example.servocontroller;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.io.OutputStream;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    TextView textAngle;
    SeekBar seekBarAngle;
    Button btnConnect;

    BluetoothAdapter bluetoothAdapter;
    BluetoothSocket bluetoothSocket;
    OutputStream outputStream;

    private final UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    String deviceAddress = "00:25:04:01:50:99";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        textAngle = findViewById(R.id.textAngle);
        seekBarAngle = findViewById(R.id.seekBarAngle);
        btnConnect = findViewById(R.id.btnConnect);

        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        btnConnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                connectBluetooth();
            }
        });

        seekBarAngle.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

                textAngle.setText("Angle: " + progress);

                sendAngle(progress);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}

        });
    }

    private void connectBluetooth() {

        try {

            BluetoothDevice device = bluetoothAdapter.getRemoteDevice(deviceAddress);

            bluetoothSocket = device.createRfcommSocketToServiceRecord(uuid);

            bluetoothSocket.connect();

            outputStream = bluetoothSocket.getOutputStream();

            Toast.makeText(this,"Bluetooth Connected",Toast.LENGTH_SHORT).show();

        } catch (Exception e) {

            Toast.makeText(this,"Connection Failed",Toast.LENGTH_SHORT).show();

            e.printStackTrace();
        }
    }

    private void sendAngle(int angle) {

        if(outputStream == null) return;

        try {

            String data = angle + "\n";

            Toast.makeText(this, "Send:" + data,Toast.LENGTH_SHORT).show();

            outputStream.write(data.getBytes());

        } catch (Exception e) {

            e.printStackTrace();
        }
    }
}