package com.wangweijun.optimize.optimize;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity  implements View.OnClickListener{

    NativeCrashMonitor nativeCrashMonitor;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        Button tv = findViewById(R.id.sample_text);
        tv.setOnClickListener(this);

        nativeCrashMonitor = new NativeCrashMonitor();
        nativeCrashMonitor.init();
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    @Override
    public void onClick(View v) {
        NativeCrashMonitor.nativeCrash();
    }

    public void nofityCondition(View view) {
        nativeCrashMonitor.nofityCondition();
    }
}