using Unity.Mathematics;
using Unity.MLAgents;
using Unity.MLAgents.Actuators;
using Unity.MLAgents.Sensors;
using Unity.VisualScripting;
using UnityEngine;
using static UnityEngine.GraphicsBuffer;

public class CarAgent : Agent
{
    public CarController carCtrl;
    public Ultrasonic[] ultrasonic;
    public Transform target;
    public CheckPoint checkPoint;
    private float prevDistance;
    private float maxDistance = 20f;

    public override void OnEpisodeBegin()
    {
        //Debug.Log("=== OnEpisodeBegin called ===");
        checkPoint.ResetIndex();
        carCtrl.Controller('S');
        this.transform.localPosition = new Vector3(0, 0, 0);
        this.transform.localRotation = Quaternion.identity;
        prevDistance = Vector3.Distance(this.transform.localPosition, target.position);
    }

    public override void CollectObservations(VectorSensor sensor)
    {
        sensor.AddObservation(ultrasonic[0].Distance / maxDistance);
        sensor.AddObservation(ultrasonic[1].Distance / maxDistance);
        sensor.AddObservation(ultrasonic[2].Distance / maxDistance);
    }

    public override void OnActionReceived(ActionBuffers actions)
    {
        int action = actions.DiscreteActions[0];
        //Debug.Log("Action received: " + action);  // 액션 값 확인

        float forward = ultrasonic[0].Distance / maxDistance;
        float left = ultrasonic[1].Distance / maxDistance;
        float right = ultrasonic[2].Distance / maxDistance;

        float currDistance = Vector3.Distance(this.transform.localPosition, target.position);
        float distanceDelta = prevDistance - currDistance;
        AddReward(distanceDelta * 0.05f);

        switch (action)
        {
            case 0:
                carCtrl.Controller('F');
                break;
            case 1:
                carCtrl.Controller('B');
                break;
            case 2:
                carCtrl.Controller('L');
                break;
            case 3:
                carCtrl.Controller('R');
                break;
        }

        if(distanceDelta < 0.005f)
        {
            AddReward(-0.1f);
        }
        if (action == 0)
        {
            AddReward(+0.005f);
        }
        if (action == 1 && forward < 0.8f)
        {
            AddReward(+0.001f);
        }
        else
        {
            AddReward(-0.001f);
        }

        if (forward > 0.8f)
        {
            if ((action == 2 && left > right) || (action == 3 && right > left))
                AddReward(+0.001f);  // 올바른 방향으로 회전
            else
                AddReward(-0.001f);  // 그냥 회전
        }

        if (MaxStep > 0) AddReward(-1.0f / MaxStep);
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Wall"))
        {
            Debug.Log("Crash!!");
            AddReward(-1.0f);
            EndEpisode();
        }

        //if (other.CompareTag("checkpoint"))
        //{
        //    //Debug.LogFormat("{0}", checkPoint.index);
        //    if (checkPoint.GetCheckPoint() == other)
        //    {
        //        checkPoint.AddIndex();
        //        AddReward(0.1f);
        //    }
        //    else
        //    {
        //        AddReward(-0.1f);
        //        //EndEpisode();
        //    }
        //}
        if (other.CompareTag("Goal"))
        {
            Debug.Log("Goal!!");
            SetReward(1.0f);
            EndEpisode();
        }

    }
}
