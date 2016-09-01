using UnityEngine;
using System.Collections;
using System.IO;
using System;

public class MagCalCalc : MonoBehaviour {
    

    // Use this for initialization
    void Start ()
    {
        FileStream magnetoReadings = new FileStream("magneto.txt", FileMode.Open);
        StreamWriter text = new StreamWriter("magnetoText.txt");
        byte[] bytes = new byte[magnetoReadings.Length];
        magnetoReadings.Read(bytes, 0, (int) magnetoReadings.Length);

        int pointCount = (int) magnetoReadings.Length / 12;
        float v;
        for(int i = 0; i < pointCount; ++i)
        {
            v = BitConverter.ToSingle(bytes, i * 12 + 0);
            text.Write(v);
            text.Write('\t');
            v = BitConverter.ToSingle(bytes, i * 12 + 4);
            text.Write(v);
            text.Write('\t');
            v = BitConverter.ToSingle(bytes, i * 12 + 8);
            text.Write(v);
            text.WriteLine();
        }



    }
	
	// Update is called once per frame
	void Update () {
	
	}
}
