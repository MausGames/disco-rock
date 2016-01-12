package at.mausgames.dr;

import org.libsdl.app.SDLActivity;
import android.os.*;


public class DiscoRock extends SDLActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        
        // fully kill application to force library reloading
        System.runFinalizersOnExit(true);
        System.exit(0);
    }
}


/*
import android.util.*;
import android.view.*;
import android.widget.*;
import android.content.res.*;
import com.google.android.gms.ads.*;


public class DiscoRock extends SDLActivity
{
  private AdView m_pAdView = null;   // ad view object
  private float m_fAdAlpha = 0.0f;   // cached alpha value

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        // get screen properties
        final int iScreenWidth = getWindowManager().getDefaultDisplay().getWidth();
        final float fDensity   = getResources().getDisplayMetrics().density;

        // create ad view
        m_pAdView = new AdView(this);
        m_pAdView.setAdSize(this.isTablet() ? AdSize.LEADERBOARD : AdSize.BANNER);
        m_pAdView.setAdUnitId("");
        if(android.os.Build.VERSION.SDK_INT >= 11)
            m_pAdView.setLayerType(View.LAYER_TYPE_HARDWARE, null);

        // create ad request
        AdRequest pAdRequest = new AdRequest.Builder()
            .addTestDevice(AdRequest.DEVICE_ID_EMULATOR)
            .addTestDevice("")
            .build();

        // set temporary callback for further initialization
        m_pAdView.setAdListener(new AdListener()
        {
            public void onAdLoaded()
            {
                // position banner in the upper center
                m_pAdView.setX((float)((iScreenWidth - m_pAdView.getAdSize().getWidth() * fDensity) / 2));
                m_pAdView.setAlpha(m_fAdAlpha);

                // add object to main view when finished
                mLayout.addView(m_pAdView);
                m_pAdView.setAdListener(null);
            }
        });

        // start loading
        m_pAdView.loadAd(pAdRequest);

    }

    @Override
    public void onResume()
    {
        super.onResume();
        if(m_pAdView != null) m_pAdView.resume();
    }

    @Override
    public void onPause()
    {
        if(m_pAdView != null) m_pAdView.pause();
        super.onPause();
    }

    @Override
    public void onDestroy()
    {
        if(m_pAdView != null) m_pAdView.destroy();
        super.onDestroy();

        // fully kill application to force library reloading
        System.runFinalizersOnExit(true);
        System.exit(0);
    }

    public void SetBannerAlpha(final float fAlpha)
    {
        if(m_pAdView == null) return;

        // cache alpha value
        m_fAdAlpha = fAlpha;

        // change banner configuration from main thread
        new Handler(Looper.getMainLooper()).post(new Runnable()
        {
            public void run()
            {
                // set alpha value of the ad
                m_pAdView.setAlpha(fAlpha);

                // disable ad when not visible
                if(fAlpha < 0.01)
                {
                    if(m_pAdView.isEnabled())
                    {
                        m_pAdView.setVisibility(View.GONE);
                        m_pAdView.setEnabled(false);
                    }
                }
                else
                {
                    if(!m_pAdView.isEnabled())
                    {
                        m_pAdView.setVisibility(View.VISIBLE);
                        m_pAdView.setEnabled(true);
                    }
                }
            }
        });
    }

    public boolean isTablet()
    {
        // check for large screens
        return (getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK)
                >= Configuration.SCREENLAYOUT_SIZE_LARGE;
    }
}
*/