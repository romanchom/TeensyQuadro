using UnityEngine;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;
using System.Text;
using System.IO;

public class Rotator : MonoBehaviour {
	string portName;
	SerialPort port;
	Thread thread;
	Object l;
	bool go = true;

    List<string> messages;
    string msg;
	Quaternion rot;
    List<Vector3> vectorData;
    Color32[] colors;

	// Use this for initialization
	void Start () {
		l = new Object();
        messages = new List<string>();
        vectorData = new List<Vector3>();
        colors = new Color32[] { Color.red, Color.yellow, Color.cyan, Color.green, Color.magenta };

		thread = new Thread(Read);

		thread.Start();
	}
	
	// Update is called once per frame
	void Update () {
		lock (l) {
			transform.localRotation = rot;
            for(int i = 0; i < vectorData.Count; ++i)
            {
                Debug.DrawRay(Vector3.zero, rot * vectorData[i] * 10, colors[i], 0.2f);
            }
        }
	}

	void Open() {
		port = new SerialPort(SerialPort.GetPortNames()[0]);
		port.Open();
	}

	void Read() {
		while (go) {
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
                    float[] fs = new float[50];
                    int id;
                    if (int.TryParse(words[0], out id))
                    {
                        for (int i = 1; i < words.Length; ++i)
                        {
                            float.TryParse(words[i], System.Globalization.NumberStyles.Any, System.Globalization.CultureInfo.InvariantCulture.NumberFormat, out fs[i - 1]);
                        }
                        lock (l)
                        {
                            while (messages.Count <= id)
                            {
                                messages.Add("");
                            }
                            messages[id] = line;
                            if (id == 100)
                            {
                                rot.w = fs[0];
                                rot.x = -fs[1];
                                rot.y = -fs[3];
                                rot.z = -fs[2];
                            }
                            if(id > 100 && id < 106)
                            {
                                while(vectorData.Count <= id - 101)
                                {
                                    vectorData.Add(new Vector3());
                                }
                                vectorData[id - 101] = new Vector3(fs[0], fs[2], fs[1]);
                            }
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

	void OnDestroy() {
		go = false;
        thread.Join();
		port.Close();
	}

	void OnGUI() {
        StringBuilder str = new StringBuilder();
        str.AppendLine(msg);
        foreach (var m in messages)
        {
            if(!string.IsNullOrEmpty(m)) str.AppendLine(m);
        }

        GUI.TextArea(new Rect(0, 0, 500, 500), str.ToString());
	}
}
