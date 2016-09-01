using UnityEngine;
using System.Collections;

public class XYZPlot : MonoBehaviour {
    [SerializeField]
    private ParticleSystem ps;

    private ParticleSystem.EmitParams p;

    void Start()
    { }

    public void addDataPoint(Vector3 pos, Color32 color)
    {
        p.position = pos;
        p.startColor = color;
        p.startSize = 0.1f;
        ps.Emit(p, 1);
    }
}
