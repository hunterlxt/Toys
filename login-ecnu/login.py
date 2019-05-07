import urllib.request
import re

##################

UN = "username"
PW = "password"

##################

postbody = "action=login&username={}&password={}&ac_id=1&user_ip=&nas_ip=&user_mac=&save_me=0&ajax=1".format(UN, PW).encode('utf-8')
retbytes = urllib.request.urlopen("https://login.ecnu.edu.cn/include/auth_action.php", postbody).read()
if (re.findall(r"login_ok".encode("utf-8"), retbytes)):
    print("Login successfully!")
else:
    print("Login failed!")
