package com.google.cloud.android.speech;

import android.os.AsyncTask;

import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URI;
import java.net.URISyntaxException;

public class HttpRequestTask extends AsyncTask<String, Void, String> {
    private String serverAdress;
    private String serverResponse = "";
//    private AlertDialog dialog;

    public HttpRequestTask(String serverAdress) {
        this.serverAdress = serverAdress;

//        dialog = new AlertDialog.Builder(context)
//                .setTitle("HTTP Response from Ip Address:")
//                .setCancelable(true)
//                .create();
    }

    @Override
    protected String doInBackground(String... params) {
//        dialog.setMessage("Data sent , waiting response from server...");
//
//        if (!dialog.isShowing())
//            dialog.show();

        String cmd = params[0];
        final String url = "http://" + serverAdress + "/cmd/" + cmd;

        try {
            HttpClient client = new DefaultHttpClient();
            HttpGet getRequest = new HttpGet();
            getRequest.setURI(new URI(url));
            HttpResponse response = client.execute(getRequest);

            InputStream inputStream = null;
            inputStream = response.getEntity().getContent();
            BufferedReader bufferedReader =
                    new BufferedReader(new InputStreamReader(inputStream));

            serverResponse = bufferedReader.readLine();
            inputStream.close();

        } catch (URISyntaxException e) {
            e.printStackTrace();
            serverResponse = e.getMessage();
        } catch (ClientProtocolException e) {
            e.printStackTrace();
            serverResponse = e.getMessage();
        } catch (IOException e) {
            e.printStackTrace();
            serverResponse = e.getMessage();
        }

        return serverResponse;
    }

}
