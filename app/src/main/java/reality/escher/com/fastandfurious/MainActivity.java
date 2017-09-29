package reality.escher.com.fastandfurious;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    static final String TAG = "FastAndFuriousJAVA";

    // Asking for permissions
    String[] accessPermissions = new String[] {
            Manifest.permission.CAMERA,
            Manifest.permission.INTERNET,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE,
    };

    // Declare UI
    SurfaceView mSurfaceView;
    SurfaceHolder mSurfaceHolder;
    Button mBlurButton;
    Button mFastButton;
    Button mFlipCameraButton;

    // Used to load the 'Fast and Furious' library on application startup.
    static {
        System.loadLibrary("FaF");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Android Permission stuff
        boolean needRequire = false;
        for(String access : accessPermissions) {
            int curPermission = ActivityCompat.checkSelfPermission(this, access);
            if (curPermission != PackageManager.PERMISSION_GRANTED) {
                needRequire = true;
                break;
            }
        }
        if (needRequire) {
            ActivityCompat.requestPermissions(this, accessPermissions, 1);
            return;
            
        }

        // send class activity and assest fd to native code
        onCreateJNI(this, getAssets());

        // Set up UI and button listeners and other Android UI jazz
        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceView);
        mSurfaceHolder = mSurfaceView.getHolder();

        mBlurButton = (Button)findViewById(R.id.blur_button);
        mBlurButton.setOnClickListener(blurListener);

        mFastButton = (Button)findViewById(R.id.fast_button);
        mFastButton.setOnClickListener(fastListener);

        mFlipCameraButton = (Button)findViewById(R.id.flip_button);
        mFlipCameraButton.setOnClickListener(flipCameraListener);

        mSurfaceHolder.addCallback(new SurfaceHolder.Callback() {

            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
                Log.v(TAG, "surfaceChanged format="+format+", width="+width+", height="+ height);
            }

            public void surfaceCreated(SurfaceHolder holder) {
                Log.v(TAG, "surfaceCreated");
                setSurface(holder.getSurface());
            }

            public void surfaceDestroyed(SurfaceHolder holder) {
                Log.v(TAG, "surfaceDestroyed");
            }
        });
    }

    // Toggles blur on and off
    private View.OnClickListener blurListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            blurToggle();
        }
    };

    // Toggles fast algo on and off
    private View.OnClickListener fastListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            fastToggle();
        }
    };

    // you like that selfie life or not?
    private View.OnClickListener flipCameraListener = new View.OnClickListener() {
        @Override
        public void onClick(View view) {
            flipCamera();
        }
    };

    // Declare all those native calls
    public native void onCreateJNI(Activity callerActivity, AssetManager assetManager);

    public native void blurToggle();

    public native void fastToggle();

    public native void flipCamera();

    // Sends surface buffer to NDK
    public native void setSurface(Surface surface);

}
