using UnityEngine;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Text;
using System.IO;
using System;

public class MagnetoCal : MonoBehaviour {
    SerialPort port;
    Thread thread;
    string msg = "";
    bool go = true;
    [SerializeField]
    private XYZPlot plot;
    List<Vector3> points;
    List<Vector3> allPoints;

    // Use this for initialization
    void Start()
    {
        points = new List<Vector3>();
        allPoints = new List<Vector3>();
        thread = new Thread(Read);
        thread.Start();
    }

    Vector3 min, max;

    // Update is called once per frame
    void Update()
    {
        lock (points)
        {
            allPoints.AddRange(points);
            foreach (Vector3 p in points)
            {
                max = Vector3.Max(max, p);
                min = Vector3.Min(min, p);
                plot.addDataPoint(p, Color.red);
            }
            points.Clear();
        }


        if (Input.GetKeyDown(KeyCode.A))
        {
            PrintCallibrationData();
        }
    }

    void Open()
    {
        port = new SerialPort(SerialPort.GetPortNames()[0]);
        port.Open();
    }

    void Read()
    {
        FileStream magnetoReadings = new FileStream("magneto.txt", FileMode.Create);
        StreamWriter text = new StreamWriter("magnetoText.txt");
        while (go)
        {
            try
            {
                if (port == null || !port.IsOpen)
                {
                    msg = "";
                    Open();
                }
                else
                {
                    string line = port.ReadLine();
                    string[] words = line.Split('\t');
                    float[] fs = new float[4];
                    for (int i = 1; i < words.Length - 1; ++i)
                    {
                        fs[i - 1] = float.Parse(words[i], System.Globalization.CultureInfo.InvariantCulture);
                    }
                    if (words[0] == "Mag")
                    {
                        text.Write(fs[0]);
                        text.Write('\t');
                        text.Write(fs[1]);
                        text.Write('\t');
                        text.Write(fs[2]);
                        text.WriteLine();
                        Vector3 p;
                        p.x = fs[0];
                        p.y = fs[2];
                        p.z = fs[1];
                        lock (points)
                        {
                            points.Add(p);
                        }
                    }
                    
                }
            }
            catch (System.Exception e)
            {
                msg = e.Message;
            }
        }
    }

    void OnDestroy()
    {
        go = false;
        thread.Join();
        port.Close();
    }

    
    void PrintCallibrationData()
    {
        Vector3 center = -(min + max) / 2;
        Vector3 scale = max - min;
        scale.x = 2.0f / scale.x;
        scale.y = 2.0f / scale.y;
        scale.z = 2.0f / scale.z;
        Debug.Log("Offset: " + center.ToString("R"));
        Debug.Log("Scale: " + scale.ToString("R"));
    }

    void OnDrawGizmos()
    {
        Gizmos.DrawWireCube((min + max) / 2, max - min);
    }
}
