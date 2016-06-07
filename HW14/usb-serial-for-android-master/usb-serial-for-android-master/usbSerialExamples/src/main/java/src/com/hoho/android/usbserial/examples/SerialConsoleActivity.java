/* Copyright 2011-2013 Google Inc.
 * Copyright 2013 mike wakerly <opensource@hoho.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 * Project home page: https://github.com/mik3y/usb-serial-for-android
 */

package com.hoho.android.usbserial.examples;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;

/////////////////// libraries for CameraApp////////////////////////////////////////
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;

///////////////////////////////////////////////////////////////////////////////

import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.util.HexDump;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Monitors a single {@link UsbSerialPort} instance, showing all data
 * received.
 *
 * @author mike wakerly (opensource@hoho.com)
 */
public class SerialConsoleActivity extends Activity implements TextureView.SurfaceTextureListener{

    private final String TAG = SerialConsoleActivity.class.getSimpleName();

    /**
     * Driver instance, passed in statically via
     * {@link #show(Context, UsbSerialPort)}.
     *
     * <p/>
     * This is a devious hack; it'd be cleaner to re-create the driver using
     * arguments passed in with the {@link #startActivity(Intent)} intent. We
     * can get away with it because both activities will run in the same
     * process, and this is a simple demo.
     */
    private static UsbSerialPort sPort = null;

    private TextView mTitleTextView;
    private TextView mDumpTextView;
    private ScrollView mScrollView;
    private CheckBox chkDTR;
    private CheckBox chkRTS;

    //added
    private SeekBar myControl; //this is seekbar for b/w threshold
    private TextView myTextView;

    ////// initializations added for CameraApp //////////////////////////////////////
    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640,480,Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private Paint paint2 = new Paint();
    private Paint paint3 = new Paint();
    private Paint paint4 = new Paint();
//    private TextView mTextView;
//    private TextView myTextView2;
//    private TextView myTextView3;
//    private SeekBar myControl1;
//    private SeekBar myControl2;
//
//    static long prevtime = 0; // for FPS calculation
/////////////////////////////////////////////////////////////////////////

    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();

    private SerialInputOutputManager mSerialIoManager;

    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {

        @Override
        public void onRunError(Exception e) {
            Log.d(TAG, "Runner stopped.");
        }

        @Override
        public void onNewData(final byte[] data) {
            SerialConsoleActivity.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    SerialConsoleActivity.this.updateReceivedData(data);
                }
            });
        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.serial_console);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        mTitleTextView = (TextView) findViewById(R.id.demoTitle);
        mDumpTextView = (TextView) findViewById(R.id.consoleText);
        mScrollView = (ScrollView) findViewById(R.id.demoScroller);
        chkDTR = (CheckBox) findViewById(R.id.checkBoxDTR);
        chkRTS = (CheckBox) findViewById(R.id.checkBoxRTS);

        //added
        myControl = (SeekBar) findViewById(R.id.seek1);
        myTextView = (TextView) findViewById(R.id.textView01);
        myTextView.setText("Hello CDC app!");
        setMyControlListener();

        ///////////////////added for camera app//////////////////////////////////
        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
        mSurfaceHolder = mSurfaceView.getHolder();
        mTextureView = (TextureView) findViewById(R.id.textureview);
        mTextureView.setSurfaceTextureListener(this);
        //mTextView = (TextView) findViewById(R.id.cameraStatus);
//        myControl1 = (SeekBar) findViewById(R.id.seek1);
//        myTextView2 = (TextView) findViewById(R.id.threshold);
//        myTextView2.setText("Threshold");
//        myControl2 = (SeekBar) findViewById(R.id.seek2);
//        myTextView3 = (TextView) findViewById(R.id.threshold2);
//        myTextView3.setText("Threshold Green");
//
        paint1.setColor(0xffff0000); // red
        paint1.setTextSize(24);
        paint2.setColor(0xffff0000); // red
        paint2.setTextSize(24);
        paint3.setColor(0xffff0000); // red
        paint3.setTextSize(24);
        paint4.setColor(0xffff0000); // red
        paint4.setTextSize(24);

//        setMyControlListener2();

        ////////////////////////////////////////////

        chkDTR.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                try {
                    sPort.setDTR(isChecked);
                }catch (IOException x){}
            }
        });

        chkRTS.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                try {
                    sPort.setRTS(isChecked);
                }catch (IOException x){}
            }
        });

    }


    @Override
    protected void onPause() {
        super.onPause();
        stopIoManager();
        if (sPort != null) {
            try {
                sPort.close();
            } catch (IOException e) {
                // Ignore.
            }
            sPort = null;
        }
        finish();
    }

    void showStatus(TextView theTextView, String theLabel, boolean theValue){
        String msg = theLabel + ": " + (theValue ? "enabled" : "disabled") + "\n";
        theTextView.append(msg);
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.d(TAG, "Resumed, port=" + sPort);
        if (sPort == null) {
            mTitleTextView.setText("No serial device.");
        } else {
            final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

            UsbDeviceConnection connection = usbManager.openDevice(sPort.getDriver().getDevice());
            if (connection == null) {
                mTitleTextView.setText("Opening device failed");
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(115200, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);

                showStatus(mDumpTextView, "CD  - Carrier Detect", sPort.getCD());
                showStatus(mDumpTextView, "CTS - Clear To Send", sPort.getCTS());
                showStatus(mDumpTextView, "DSR - Data Set Ready", sPort.getDSR());
                showStatus(mDumpTextView, "DTR - Data Terminal Ready", sPort.getDTR());
                showStatus(mDumpTextView, "DSR - Data Set Ready", sPort.getDSR());
                showStatus(mDumpTextView, "RI  - Ring Indicator", sPort.getRI());
                showStatus(mDumpTextView, "RTS - Request To Send", sPort.getRTS());

                int i = 100;
                String sendString = String.valueOf(i) + '\n';
                try {
                    sPort.write(sendString.getBytes(),10); // 10 is the timeout
                }
                catch (IOException e) {}


            } catch (IOException e) {
                Log.e(TAG, "Error setting up device: " + e.getMessage(), e);
                mTitleTextView.setText("Error opening device: " + e.getMessage());
                try {
                    sPort.close();
                } catch (IOException e2) {
                    // Ignore.
                }
                sPort = null;
                return;
            }
            mTitleTextView.setText("Serial device: " + sPort.getClass().getSimpleName());
        }
        onDeviceStateChange();
    }

    private void stopIoManager() {
        if (mSerialIoManager != null) {
            Log.i(TAG, "Stopping io manager ..");
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if (sPort != null) {
            Log.i(TAG, "Starting io manager ..");
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange() {
        stopIoManager();
        startIoManager();
    }

    private void updateReceivedData(byte[] data) {
        final String message = "Read " + data.length + " bytes: \n"
                + HexDump.dumpHexString(data) + "\n\n";
        mDumpTextView.append(message);
        mScrollView.smoothScrollTo(0, mDumpTextView.getBottom());
    }

    /**
     * Starts the activity, using the supplied driver instance.
     *
     * @param context
     * @param driver
     */
    static void show(Context context, UsbSerialPort port) {
        sPort = port;
        final Intent intent = new Intent(context, SerialConsoleActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP | Intent.FLAG_ACTIVITY_NO_HISTORY);
        context.startActivity(intent);
    }
    private void setMyControlListener() {
        myControl.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;
                //myTextView.setText("The value is: "+progress);
                //float thresh;
                //thresh = myControl.getProgress();
                //myTextView.setText("The threshold is: "+thresh);
                //int i = 100;
//                String sendString = String.valueOf(thresh) + '\n';
//                try {
//                    sPort.write(sendString.getBytes(),10); // 10 is the timeout
//                }
//                catch (IOException e) {}


            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
    }

    //////////////////added second listener function ///////////////////////////////////////////////
//    private void setMyControlListener2() {
//        myControl2.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
//
//            int progressChanged2 = 0;
//
//            @Override
//            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
//                progressChanged2 = progress;
//
//
//            }
//
//            @Override
//            public void onStartTrackingTouch(SeekBar seekBar) {
//            }
//
//            @Override
//            public void onStopTrackingTouch(SeekBar seekBar) {
//
//            }
//        });
//    }
//
//
//    ////////////////// more camera functions/////////////////////////////////////////////////
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        //parameters.setColorEffect(Camera.Parameters.EFFECT_MONO); // black and white
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // Invoked every time there's a new Camera preview frame
        mTextureView.getBitmap(bmp);
//        //setMyControlListener();
        float thresh;
        thresh = myControl.getProgress()*(800/100);
        myTextView.setText("The blck/wht threshold is: "+thresh);

//        String sendString = String.valueOf(thresh) + '\n';
//        try {
//            sPort.write(sendString.getBytes(),10); // 10 is the timeout
//        }
//        catch (IOException e) {}
//
//        float thresh_gn;
//        thresh_gn = myControl2.getProgress()*(300/100);
//        myTextView3.setText("The green threshold is: "+thresh_gn);
//
        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {

            int[] pixels = new int[bmp.getWidth()];
            int[] pixels2 = new int[bmp.getWidth()];
            int[] pixels3 = new int[bmp.getWidth()];
            int[] pixels4 = new int[bmp.getWidth()];
            int startY = 30; // which row in the bitmap to analyse to read
            int startY2 = 300;
            int startY3 = 200;
            int startY4 = 400;

            // only look at one row in the image
            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1); // (array name, offset inside array, stride (size of row), start x, start y, num pixels to read per row, num rows to read)
            bmp.getPixels(pixels2, 0, bmp.getWidth(), 0, startY2, bmp.getWidth(), 1);
            bmp.getPixels(pixels3, 0, bmp.getWidth(), 0, startY3, bmp.getWidth(), 1);
            bmp.getPixels(pixels4, 0, bmp.getWidth(), 0, startY4, bmp.getWidth(), 1);
            // pixels[] is the RGBA data (in black an white).
            // instead of doing center of mass on it, decide if each pixel is dark enough to consider black or white
            // then do a center of mass on the thresholded array
            int[] thresholdedPixels = new int[bmp.getWidth()];
            int[] thresholdedPixels2 = new int[bmp.getWidth()];
            int[] thresholdedPixels3 = new int[bmp.getWidth()];
            int[] thresholdedPixels4 = new int[bmp.getWidth()];

            int[] thresholdedColors = new int[bmp.getWidth()];

            int wbTotal = 0; // total mass
            int wbCOM = 0; // total (mass time position)
            int wbTotal2 = 0;
            int wbCOM2 = 0;
            int wbTotal3 = 0;
            int wbCOM3 = 0;
            int wbTotal4 = 0;
            int wbCOM4 = 0;

            for (int i = 0; i < bmp.getWidth(); i++) {
                // sum the red, green and blue, subtract from 255 to get the darkness of the pixel.
                // if it is greater than some value (600 here), consider it black
                // play with the 600 value if you are having issues reliably seeing the line

                if (255 * 3 - (red(pixels[i]) + green(pixels[i]) + blue(pixels[i])) > 600) {//thresh
                    thresholdedPixels[i] = 255 * 3;

                } else {
                    thresholdedPixels[i] = 0;
                }
                if (255-(green(pixels[i])-red(pixels[i])) > 252){///thresh_gn
                    thresholdedColors[i]=rgb(0,0,0);//black
                }else{
                    thresholdedColors[i]=rgb(0,255,0);//green

                }

                wbTotal = wbTotal + thresholdedPixels[i];
                wbCOM = wbCOM + thresholdedPixels[i] * i;

                if (255 * 3 - (red(pixels2[i]) + green(pixels2[i]) + blue(pixels2[i])) > 600) {
                    thresholdedPixels2[i] = 255 * 3;
                } else {
                    thresholdedPixels2[i] = 0;
                }
                wbTotal2 = wbTotal2 + thresholdedPixels2[i];
                wbCOM2 = wbCOM2 + thresholdedPixels2[i] * i;

                if (255 * 3 - (red(pixels3[i]) + green(pixels3[i]) + blue(pixels3[i])) > 600) {
                    thresholdedPixels3[i] = 255 * 3;
                } else {
                    thresholdedPixels3[i] = 0;
                }
                wbTotal3 = wbTotal3 + thresholdedPixels3[i];
                wbCOM3 = wbCOM3 + thresholdedPixels3[i] * i;

                if (255 * 3 - (red(pixels4[i]) + green(pixels4[i]) + blue(pixels4[i])) > 600) {
                    thresholdedPixels4[i] = 255 * 3;
                } else {
                    thresholdedPixels4[i] = 0;
                }
                wbTotal4 = wbTotal4 + thresholdedPixels4[i];
                wbCOM4 = wbCOM4 + thresholdedPixels4[i] * i;
            }
//////////////////////////////////////////////////////////////////////////////////////////////
            bmp.setPixels(thresholdedColors, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);
            int COM;
            //watch out for divide by 0
            if (wbTotal<=0) {
                COM = bmp.getWidth()/2;
            }
            else {
                COM = wbCOM/wbTotal;
            }
            int COM2;
            //watch out for divide by 0
            if (wbTotal2<=0) {
                COM2 = bmp.getWidth()/2;
            }
            else {
                COM2 = wbCOM2/wbTotal2;
            }
            int COM3;
            //watch out for divide by 0
            if (wbTotal3<=0) {
                COM3 = bmp.getWidth()/2;
            }
            else {
                COM3 = wbCOM3/wbTotal3;
            }
            int COM4;
            //watch out for divide by 0
            if (wbTotal4<=0) {
                COM4 = bmp.getWidth()/2;
            }
            else {
                COM4 = wbCOM4/wbTotal4;
            }

            // draw a circle where you think the COM is
            canvas.drawCircle(COM, startY, 5, paint1);
            canvas.drawCircle(COM2, startY2, 5, paint2);
            canvas.drawCircle(COM3, startY3, 5, paint3);
            canvas.drawCircle(COM4, startY4, 5, paint4);

            // also write the value as text
            canvas.drawText("COM1 = " + COM, 10, 300, paint1);
            canvas.drawText("COM2 = " + COM2, 10, 350, paint2);
            canvas.drawText("COM3 = " + COM3, 10, 400, paint3);
            canvas.drawText("COM4 = " + COM4, 10, 450, paint4);
            c.drawBitmap(bmp, 0, 0, null);
            mSurfaceHolder.unlockCanvasAndPost(c);

            //calculate
            int difference;
            difference = COM3-COM4;
            String sendString = String.valueOf(difference) + '\n';
            try {
                sPort.write(sendString.getBytes(),10); // 10 is the timeout
            }
            catch (IOException e) {}

//            // calculate the FPS to see how fast the code is running
//            long nowtime = System.currentTimeMillis();
//            long diff = nowtime - prevtime;
//            mTextView.setText("FPS " + 1000/diff);
//            prevtime = nowtime;
        }
    }
}