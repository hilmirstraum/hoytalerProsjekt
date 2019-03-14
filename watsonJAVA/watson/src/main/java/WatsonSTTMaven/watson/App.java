package WatsonSTTMaven.watson;

import java.io.File;
import java.io.FileNotFoundException;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.TargetDataLine;

import com.ibm.watson.developer_cloud.http.HttpMediaType;
import com.ibm.watson.developer_cloud.service.security.IamOptions;
import com.ibm.watson.developer_cloud.speech_to_text.v1.SpeechToText;
import com.ibm.watson.developer_cloud.speech_to_text.v1.model.RecognizeOptions;
import com.ibm.watson.developer_cloud.speech_to_text.v1.model.SpeechRecognitionResults;
import com.ibm.watson.developer_cloud.speech_to_text.v1.websocket.BaseRecognizeCallback;

/**
 * Hello world!
 *
 */
public class App 
{
	
	private static final String APIKEY = "uhz1dA7qYO07JFpStAyne-F60CcMAyWhRRlrnBv-emZO";
	private static final String APIURL = "https://gateway-lon.watsonplatform.net/speech-to-text/api";
	
	
    public static void main( String[] args ) throws FileNotFoundException, InterruptedException, LineUnavailableException
    {
        System.out.println( "Hello World!" );
        
        IamOptions options = new IamOptions.Builder()
        	    .apiKey(APIKEY)
        	    .build();

        SpeechToText speechToText = new SpeechToText(options);
        speechToText.setEndPoint(APIURL);

        int sampleRate = 16000;
        AudioFormat format = new AudioFormat(sampleRate, 16, 1, true, false);
        DataLine.Info info = new DataLine.Info(TargetDataLine.class, format);

        if (!AudioSystem.isLineSupported(info)) {
          System.out.println("Line not supported");
          System.exit(0);
        }

        TargetDataLine line = (TargetDataLine) AudioSystem.getLine(info);
        line.open(format);
        line.start();

        AudioInputStream audio = new AudioInputStream(line);

        RecognizeOptions options1 = new RecognizeOptions.Builder()
          .interimResults(true)
        //.inactivityTimeout(5) // use this to stop listening when the speaker pauses, i.e. for 5s
          .audio(audio)
          .contentType(HttpMediaType.AUDIO_RAW + "; rate=" + sampleRate)
          .build();

        speechToText.recognizeUsingWebSocket(options1, new BaseRecognizeCallback() {
          @Override
          public void onTranscription(SpeechRecognitionResults speechResults) {
            System.out.println(speechResults);
          }
        });

        System.out.println("Listening to your voice for the next 10s...");
        Thread.sleep(10 * 1000);

        // closing the WebSockets underlying InputStream will close the WebSocket itself.
        line.stop();
        line.close();

        System.out.println("Fin.");
    }
}
