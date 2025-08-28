using System.Collections.Generic;
using UnityEngine;

public class CheckPoint : MonoBehaviour
{
    static int _index = 0;
    public int index {  get { return _index; } }
    public Collider[] CheckPoints { get; private set; }

    void Start()
    {
        CheckPoints = GetComponentsInChildren<Collider>();
        ResetIndex();
    }

    public void ResetIndex()
    {
        _index = 0;
    }

    public void AddIndex()
    {
        _index++;
    }

    public Collider GetCheckPoint()
    {
        return CheckPoints[_index];
    }
}
