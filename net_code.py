from flask import Flask,jsonify
from flask_mqtt import Mqtt
import datetime
from pymysql import Connection
from flask_cors import CORS


con = Connection(  # 创建连接
    host="123.60.130.168",
    port=3306,
    user="data",
    password="LnxSbb7pPD2tmhrA",
    database="data"
)
print(type(con))
print(con.get_host_info())
print(con.get_server_info())

# 创建游标
cursor = con.cursor()

app = Flask(__name__)
CORS(app,resources=r'/*',supports_credentials=True)
app.config['MQTT_BROKER_URL'] = '123.60.130.168'
app.config['MQTT_BROKER_PORT'] = 1883
# app.config['MQTT_USERNAME'] = ''  # 当你需要验证用户名和密码时，请设置该项
# app.config['MQTT_PASSWORD'] = ''  # 当你需要验证用户名和密码时，请设置该项
app.config['MQTT_KEEPALIVE'] = 5  # 设置心跳时间，单位为秒
app.config['MQTT_TLS_ENABLED'] = False  # 如果你的服务器支持 TLS，请设置为 True
topic = 'pubtopic'

mqtt_client = Mqtt(app)


@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
    if rc == 0:
        print('Connected successfully')
        mqtt_client.subscribe(topic)  # 订阅主题
    else:
        print('Bad connection. Code:', rc)


@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
    global payload, second
    now = datetime.datetime.now()
    second = now.second  # 当前时间的秒数
    minute = now.minute  # 获取当前分钟数
    payload = message.payload.decode()
    print('%s' % payload)
    # 写入数据库
    sql = """
        INSERT INTO table1(payload,minute,second) VALUES("%s","%d","%d" )
    """ % (payload, minute, second)
    rows = cursor.execute(sql)
    print("db:%d" % rows)
    con.commit()
    return payload


@app.route("/api")
def http():
    sql = "select * from table1;"
    cursor.execute(sql)
    # 可迭代对象
    payback = cursor.fetchall()
    output_num = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    output_time = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    # 循环结果集
    for i in range(-15, 0):
        output_num[i] = payback[i][1]
        output_time[i] = payback[i][3]

    time =[
        output_time[0], output_time[1], output_time[2], output_time[3], output_time[4],
        output_time[5], output_time[6], output_time[7], output_time[8], output_time[9],
        output_time[10], output_time[11], output_time[12], output_time[13], output_time[14]]
    value = [
        output_num[0], output_num[1], output_num[2], output_num[3], output_num[4],
        output_num[5], output_num[6], output_num[7], output_num[8], output_num[9],
        output_num[10], output_num[11], output_num[12], output_num[13], output_num[14]]
    return jsonify({
        "data":{
            "time":time,
            "value":value,
        }
    })
'''
    return jsonify ({"data":{
        "time":"[%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s]"%(
        output_time[0], output_time[1], output_time[2], output_time[3], output_time[4],output_time[5], output_time[6], output_time[7], output_time[8], output_time[9],output_time[10], output_time[11], output_time[12], output_time[13], output_time[14]),
        "value":"[%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s]"%(output_num[0], output_num[1], output_num[2], output_num[3], output_num[4],
        output_num[5], output_num[6], output_num[7], output_num[8], output_num[9],
        output_num[10], output_num[11], output_num[12], output_num[13], output_num[14]),
    }
    }

    )'''
'''
        "data:{time:[%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s],value:[%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s]}" % (
        output_time[0], output_time[1], output_time[2], output_time[3], output_time[4],
        output_time[5], output_time[6], output_time[7], output_time[8], output_time[9],
        output_time[10], output_time[11], output_time[12], output_time[13], output_time[14],
        output_num[0], output_num[1], output_num[2], output_num[3], output_num[4],
        output_num[5], output_num[6], output_num[7], output_num[8], output_num[9],
        output_num[10], output_num[11], output_num[12], output_num[13], output_num[14]))
'''

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
