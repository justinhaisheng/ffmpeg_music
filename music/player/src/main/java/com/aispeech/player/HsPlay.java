package com.aispeech.player;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.text.TextUtils;
import android.util.Log;

import com.aispeech.HsTimeInfoBean;
import com.aispeech.listener.HsCompleteListener;
import com.aispeech.listener.HsErrorListener;
import com.aispeech.listener.HsLoadingListener;
import com.aispeech.listener.HsOnTimeInfoListener;
import com.aispeech.listener.HsPrepareListener;
import com.aispeech.listener.HsValumeDBListener;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * @创建者 luhaisheng
 * @创建时间 2020/5/11 11:49
 * @描述
 */
public class HsPlay {

    public final static String TAG = "HsPlay";

    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avdevice-57");
        System.loadLibrary("avfilter-6");
        System.loadLibrary("avformat-57");
        System.loadLibrary("avutil-55");
        System.loadLibrary("postproc-54");
        System.loadLibrary("swresample-2");
        System.loadLibrary("swscale-4");
    }

    private String mSource;//数据源
    private boolean mNextSource = false;//下一个数据源

    public void setSource(String source) {
        this.mSource = source;
    }

    public void setNextSource(String nextSource) {
        if (!TextUtils.isEmpty(nextSource)) {
            this.mSource = nextSource;
            mNextSource = true;
        } else {
            mNextSource = false;
        }
    }

    public boolean getNextSource() {
        return mNextSource;
    }


    HsPrepareListener mHsPrepareListener;

    public void setHsPrepareListener(HsPrepareListener hsPrepareListener) {
        this.mHsPrepareListener = hsPrepareListener;
    }

    HsLoadingListener mHsLoadListener;

    public void setHsLoadListener(HsLoadingListener hsLoadListener) {
        this.mHsLoadListener = hsLoadListener;
    }

    HsOnTimeInfoListener mHsOnTimeInfoListener;

    public void setHsOnTimeInfoListener(HsOnTimeInfoListener hsOnTimeInfoListener) {
        this.mHsOnTimeInfoListener = hsOnTimeInfoListener;
    }

    HsErrorListener mHsErrorListener;

    public void setHsErrorListener(HsErrorListener hsErrorListener) {
        this.mHsErrorListener = hsErrorListener;
    }

    HsCompleteListener mHsCompleteListener;

    public void setHsCompleteListener(HsCompleteListener hsCompleteListener) {
        this.mHsCompleteListener = hsCompleteListener;
    }

    HsValumeDBListener mHsValumeDBListener;

    public void setHsValumeDBListener(HsValumeDBListener hsValumeDBListener) {
        this.mHsValumeDBListener = hsValumeDBListener;
    }


    public void prepare() {
        if (TextUtils.isEmpty(mSource)) {
            Log.e(TAG, "prepare mSource == NULL");
            return;
        }
        onCallLoading(true);//正在加载
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_prepare(mSource);
            }
        }).start();
    }

    public void start() {
        if (TextUtils.isEmpty(mSource)) {
            Log.e(TAG, "start mSource == NULL");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_start();
            }
        }).start();
    }

    public void pause() {
        if (TextUtils.isEmpty(mSource)) {
            Log.e(TAG, "start pause == NULL");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_pause();
            }
        }).start();
    }

    public void resume() {
        if (TextUtils.isEmpty(mSource)) {
            Log.e(TAG, "start resume == NULL");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_resume();
            }
        }).start();
    }

    public void stop() {
        onCallTimeInfo(0, 0);
        hsTimeInfoBean = null;
        stopRecord();
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_stop();
            }
        }).start();
    }

    public void seek(final int seconds) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_seek(seconds);
            }
        }).start();
    }

    public void seekVolume(final int progress) {
        n_seekVolume(progress);
    }

    public void setMute(final MuteEnum mute) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_mute(mute.getValue());
            }
        }).start();
    }

    public void setPitch(float p) {
        n_pitch(p);
    }

    public void setSpeed(float s) {
        n_speed(s);
    }

    public int getDuration() {
        if (hsTimeInfoBean != null) {
            return hsTimeInfoBean.getTotalTime();
        }
        return 0;
    }

    //开始录制
    private boolean initmediacodec = false;
    public void startRecord(File outfile) {
        if (initmediacodec)
            return;
        int sampleSate = n_samplerate();
        if(sampleSate > 0) {
            initmediacodec = true;
            n_startstoprecord(true);
            initMediaCodec(sampleSate, outfile);
        }
    }

    //停止录制
    public void stopRecord(){
        if (!initmediacodec)
            return;
        n_startstoprecord(false);
        releaseMediaCodec();
        initmediacodec = false;
    }

    public void pauseRecord() {
        n_startstoprecord(false);
    }

    public void resumeRcord() {
        n_startstoprecord(true);
    }

    private void releaseMediaCodec() {
        if (mAudioEncoder == null)
            return;
        mAudioEncoder.stop();
        mAudioEncoder = null;
        mAudioFormat = null;
        if (mOutputStream!=null){
            try {
                mOutputStream.close();
            } catch (IOException ex) {
                ex.printStackTrace();
            }
        }
    }


    /*
     *C++ 回调 java的方法
     *@author luhaisheng
     *@time 2020/5/11 12:15
     */
    private void onCallPrepare() {
        if (mHsPrepareListener != null) {
            mHsPrepareListener.prepare();
        }
    }

    /*
     *C++ 回调 java的方法
     *@author luhaisheng
     *@time 2020/5/11 12:15
     */
    private void onCallLoading(boolean load) {
        if (mHsLoadListener != null) {
            mHsLoadListener.loading(load);
        }
    }

    private static HsTimeInfoBean hsTimeInfoBean;

    private void onCallTimeInfo(int currentTime, int totalTime) {
        if (mHsOnTimeInfoListener != null) {
            if (hsTimeInfoBean == null) {
                hsTimeInfoBean = new HsTimeInfoBean();
            }
            hsTimeInfoBean.setCurrentTime(currentTime);
            hsTimeInfoBean.setTotalTime(totalTime);
            mHsOnTimeInfoListener.onTimeInfo(hsTimeInfoBean);
        }
    }

    private void onCallError(int code, String codemsg) {
        stop();
        if (mHsErrorListener != null) {
            mHsErrorListener.error(code, codemsg);
        }
    }

    private void onCallComplete() {
        stop();
        if (mHsCompleteListener != null) {
            mHsCompleteListener.complete();
        }
    }

    private void onCallValumeDB(int db) {
        if (mHsValumeDBListener != null) {
            mHsValumeDBListener.onDbValue(db);
        }
    }


    private native void n_prepare(String source);

    private native void n_start();

    private native void n_resume();

    private native void n_pause();

    private native void n_stop();

    private native void n_seek(int sencods);

    private native void n_seekVolume(int volume);

    private native void n_mute(int mute);

    private native void n_pitch(float pitch);

    private native void n_speed(float speed);

    private native int n_samplerate();

    private native void n_startstoprecord(boolean record);

    //mediacodec
    private MediaCodec mAudioEncoder;
    private MediaFormat mAudioFormat;
    private MediaCodec.BufferInfo mBufferInfo;
    private FileOutputStream mOutputStream;
    private int mAacSamperate = 0;

    //初始化编码器
    private void initMediaCodec(int samperate, File outfile) {
        try {
            //创建音频编码器
            mAudioEncoder = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);
            if (mAudioEncoder == null) {
                Log.e(TAG, "create audio encoder failure");
                return;
            }
            mAacSamperate = getADTSsamplerate(samperate);
            mAudioFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, samperate, 2);
            mAudioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 96000);//码率
            mAudioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            mAudioFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE, 10240);
            mAudioEncoder.configure(mAudioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mBufferInfo = new MediaCodec.BufferInfo();//编码后的信息，preTime
            mOutputStream = new FileOutputStream(outfile);
            mAudioEncoder.start();//编码器启动
        } catch (IOException e) {
            e.printStackTrace();
            Log.e(TAG, "create audio encoder failure e" + e.getMessage());
            mAudioEncoder = null;
            if (mOutputStream!=null){
                try {
                    mOutputStream.close();
                } catch (IOException ex) {
                    ex.printStackTrace();
                }
            }
        }
    }

    private void encodecPcmToAAc(byte[] data, int size) {
        if (mAudioEncoder == null) return;
        if (data == null) return;
        Log.d(TAG, "encodecPcmToAAc() size:"+size+" "+data.length);
        int inputIndex = mAudioEncoder.dequeueInputBuffer(0);//获取输入队列的可使用index
        if (inputIndex >= 0) {
            ByteBuffer inputBuffer = mAudioEncoder.getInputBuffers()[inputIndex];
            //Log.d(TAG, "encodecPcmToAAc()11 "+inputBuffer.capacity());

            inputBuffer.clear();
            inputBuffer.put(data);
            mAudioEncoder.queueInputBuffer(inputIndex, 0, size, 0, 0);//输入到队列
        }else{
            Log.e(TAG, "encodecPcmToAAc inputIndex<0");
            return;
        }
        //开始获取编码后的数据
        int outputIndex = mAudioEncoder.dequeueOutputBuffer(mBufferInfo, 0);//获取输出队列的可使用index
        while (outputIndex >= 0) {
            try {
                int perpcmsize = mBufferInfo.size + 7;//7 adts的头字节数
                byte[] outByteBuffer = new byte[perpcmsize];
                ByteBuffer outBuffer = mAudioEncoder.getOutputBuffers()[outputIndex];
                outBuffer.position(mBufferInfo.offset);
                outBuffer.limit(mBufferInfo.offset + mBufferInfo.size);
                addADtsHeader(outByteBuffer, perpcmsize, mAacSamperate);//添加adts 头

                outBuffer.get(outByteBuffer, 7, mBufferInfo.size);
                outBuffer.position(mBufferInfo.offset);
                //写入到文件
                mOutputStream.write(outByteBuffer, 0, perpcmsize);
                mAudioEncoder.releaseOutputBuffer(outputIndex, false);
                outputIndex = mAudioEncoder.dequeueOutputBuffer(mBufferInfo, 0);
                outByteBuffer = null;
            } catch (IOException e) {
                e.printStackTrace();
                Log.e(TAG, "encodecPcmToAAc() e:"+e.getMessage());
                continue;
            }

        }


    }

    private int getADTSsamplerate(int samplerate) {
        int rate = 4;
        switch (samplerate) {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }

    private void addADtsHeader(byte[] packet, int packetLen, int samplerate) {
        int profile = 2; // AAC LC
        int freqIdx = samplerate; // samplerate
        int chanCfg = 2; // CPE

        packet[0] = (byte) 0xFF; // 0xFFF(12bit) 这里只取了8位，所以还差4位放到下一个里面
        packet[1] = (byte) 0xF9; // 第一个t位放F
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }


}
