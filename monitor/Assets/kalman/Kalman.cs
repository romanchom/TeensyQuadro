using UnityEngine;
using System.Collections;

public class Kalman : MonoBehaviour {
    [SerializeField]
    private Plot plot;

	// Use this for initialization
	void Start ()
    {
        float estimate = 0;
        const float errorInMeasurement = 0.3f;
        float errorInEstimate = 10.0f;

        for (int i = 0; i < 300; ++i)
        {
            float kalmanGain = KalmanGain(errorInEstimate, errorInMeasurement);
            float measurement = GetMeasurement(i);
            estimate = NextEstimate(estimate, kalmanGain, measurement);
            errorInEstimate = NextErrorInEstimate(kalmanGain, errorInEstimate);

            plot.addDataPoint(measurement, i, Color.red);
            plot.addDataPoint(estimate, i, Color.green);
        }
	}


    float GetMeasurement(int index)
    {
        const float r = 0.8f;
        return 2 + Mathf.Sin(index * 0.2f) + Random.Range(-r, r) * Random.Range(-r, r);
    }

	// Update is called once per frame
	void Update () {
	
	}

    float KalmanGain(float errorInEstimate, float errorInMeasurement)
    {
        return errorInEstimate / (errorInEstimate + errorInMeasurement);
    }

    float NextEstimate(float previousEstimate, float kalmanGain, float measurement)
    {
        return previousEstimate + kalmanGain * (measurement - previousEstimate);
    }

    float NextErrorInEstimate(float kalmanGain, float previousErrorInEstimate)
    {
        return (1 - kalmanGain) * previousErrorInEstimate;
    }
}
