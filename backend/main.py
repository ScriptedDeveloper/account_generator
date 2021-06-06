try:
    from requests import Session
    import json
    from flask import Flask, request, Response, jsonify
    from threading import Thread
    from os import stat, remove
    from flask_limiter import Limiter
    from flask_limiter.util import get_remote_address
    from random import choice
    from string import digits, ascii_letters
    from sys import stderr
    from flaskthreads import AppContextThread
except:
    from os import system
    system('pip install requests')
    system('pip install flask_limiter')
    system('pip install threading')
    exit()

app = Flask(__name__)
limiter = Limiter(app, key_func=get_remote_address, default_limits=['4 per minute'])

class API():
    def __init__(self):
        self.whitelists = json.load(open('whitelists.json'))
        self.accounts = json.load(open('accounts.json'))
        self.admins = json.load(open('admins.json'))
        self.plans = json.load(open('plans.json'))
        self.empty = False

    def json_dump_account(self):
        json.dump(self.accounts, open('accounts.json', 'w'))

    def json_dump_admins(self):
        json.dump(self.admins, open('admins.json', 'w'))
    
    def json_dump_plans(self):
        json.dump(self.plans, open('plans.json'))

    def json_dump_whitelists(self):
        json.dump(self.whitelists, open('whitelists.json', 'w'))

    def check_stock(self):
        while True:
            acc = 0
            for acc_ in open("accounts.txt", "r").readlines():
                try:
                    for acc__ in open(str(acc_) + ".txt", 'r').readlines():
                        acc = acc + 1
                        self.accounts[acc_][0]["stock"] = acc 
                        self.json_dump_account()
                except OSError:pass

    def start_check_stock(self):
        Thread(target=self.check_stock).start()

    def check_if_txt_created(self, a_type):
        open(a_type + ".txt", 'r')

    def check_if_empty(self, a_type):
        if stat(a_type + ".txt").st_size != 0:
            self.empty = False
        else:
            self.empty = True

    def main(self):
        @app.route('/accounts', methods=['GET'])
        @limiter.limit("4 per minute")
        def response_call():
            try:
                if not self.whitelists[request.headers.get('auth')]:
                    return Response(status=403)

                else:return jsonify({"account_type" : open("accounts.txt", 'r').read(), "status" : 200}) 
            except KeyError:pass

            try:
                if not self.admins[request.headers.get('auth')]:
                    return Response(status=403)
                else:return jsonify({"account_type" : open("accounts.txt", 'r').read(), "status" : 200})

            except KeyError:return Response(status=403)
        
        @app.route('/get/account', methods=['GET'])
        @limiter.exempt
        def response_call1():
            try:
                if self.whitelists[request.headers.get('auth')] == "":pass
            except KeyError:
                pass
            try:
                if self.admins[request.headers.get('auth')] == "":pass
            except KeyError:
                return Response(status=403)
            try:
                Thread(target=self.check_if_txt_created(request.headers.get("account_type"))).start()
                Thread(target=self.check_if_empty(request.headers.get("account_type"))).start()
            except FileNotFoundError:
                return Response(status=404)
            if self.empty == True:
                return jsonify({'status' : 400, 'reason' : 'empty'})
            else:
                account = open(request.headers.get("account_type") + '.txt', 'r').readline()
                open(request.headers.get("account_type") + 'txt', 'w').writelines(open(request.headers.get("account_type") +'.txt', 'r').read().splitlines(True)[1:])
                self.accounts[request.headers.get("account_type")][0]["stock"] -= 1 # This is how you do it
                self.json_dump_account()
                return jsonify({'status' : 200, 'account' : account})
            return Response(status=403)

        @app.route('/auth', methods=['GET'])
        @limiter.limit('20 per minute')
        def response_call2():
            key = request.headers.get("auth")
            try:
                if self.whitelists[request.headers.get("auth")] != "":
                    return jsonify({"is_admin" : False, "status" : 200})
                else:return Response(status=403)
            except KeyError:pass
            try:
                if self.admins[request.headers.get("auth")] != "":
                    return jsonify({"is_admin" : True, "status" : 200})
                else:return Response(status=403)
            except KeyError:pass
            return Response(status=403)

        
        # Admin commands

        @app.route('/admin/add_account_type', methods=["GET"])
        @limiter.exempt
        def response_call3():
           # try:
                for admin in self.admins:
                    if admin == request.headers.get('auth'):
                        open(request.headers.get("name") + '.txt', 'w+')
                        open("accounts.txt", 'w').write("\n" + request.headers.get("name"))
                        new_plan = {request.headers.get("name") : [{"stock" : 0, "frozen" : False}]}
                        self.accounts.update(new_plan)
                        open("accounts.json", 'r+').seek(0)
                        self.json_dump_account()
                        return Response(status=200)

                    else:pass

                return Response(status=403)

         #   except TypeError:return jsonify({'error' : 'invalid auth', 'status' : 403})

        @app.route('/admin/remove_account_type')
        @limiter.exempt
        def response_call4():
            for admin_ in self.admins:
                try:
                    if admin == request.headers.get('auth'):
                        open(request.json["name"] + '.txt')
                        remove("errr.txt")
                        for line in open("accounts.txt", 'r').read():
                            if line == request.headers.get("name"):
                                open("accounts.txt", 'w').writelines(open("accounts.txt", 'r').read().splitlines(True)[1:])
                            else:pass
                        self.accounts[request.json['name']] = ""
                        self.json_dump_account()
                        return Response(status=200)

                    else:return Response(status=403)

                except TypeError:
                    return jsonify({'error' : 'invalid auth', 'status' : 403})



        @app.route('/admin/lock_account_type')
        @limiter.exempt
        def response_call5():
            try:
                if self.admins[request.headers.get("auth")] != "false" or not self.admins[request.headers.get("auth")]:
                    if self.accounts[request.headers.get("account_types")]['frozen'] != False:
                        self.accounts[request.json["account_types"]]['frozen'] = True
                        self.json_dump_account()
                        return Response(status=200)

                    else:return jsonify({"status" : 400, "reason" : "Already locked"})

                else:return Response(status=403)

            except TypeError:
                    return jsonify({'error' : 'invalid auth', 'status' : 403})
        
        @app.route('/admin/unlock_account_type')
        @limiter.exempt
        def response_call6():
            for admin_ in self.admins:
                try:
                    if admin_ == request.json['auth']:
                        for accounts in self.accounts['account_types']:
                            if self.accounts[request.json["account_types"]]['frozen'] != True:
                                self.accounts[request.json["account_types"]]['frozen'] = False
                                self.json_dump_account()
                                return Response(status=200)

                            else:return jsonify({"status" : 400, "reason" : "Already unlocked"})

                    else:return Response(status=403)

                except TypeError:
                    return jsonify({'error' : 'invalid auth', 'status' : 403})
            
        @app.route('/admin/create/whitelist', methods=["GET"])
        @limiter.limit('100 per minute')
        def response_call7():
            try:
                if not admin_[request.headers.get("auth")]:
                        if request.headers.get['plan'] == self.plans['emerald']:
                            whitelist = ''.join(choice(ascii_letters + digits) for x in range(32))
                            self.whitelists[whitelist] = 10

                        else:self.whitelists[whitelist] = 30
                        self.json_dump_plans()
                        return jsonify({"whitelist" : whitelist, "status" : 200})

            except TypeError:
                return jsonify({'error' : 'invalid_auth', 'status' : 403})


        @app.route('/admin/remove/whitelist', methods=['GET'])
        @limiter.limit('100 per minute')
        def response_call8():
            for admin_ in self.admins:
                try:
                    if admin_ == request.headers.get('auth'):
                        for whitelist in self.whitelists:
                            try:
                                if whitelist == request.headers.get('whitelist'):
                                    del whitelist
                            except:
                                return jsonify({'error' : 'Whitelist not found', 'status' : 404})
                        self.json_dump_plans()
                    return Response(status=200)
                except TypeError:
                    return jsonify({'error' : 'invalid auth', 'status' : 403})

        @app.route('/admin/create/plan', methods=['POST'])
        @limiter.exempt
        def response_call9():
            for admin_ in self.admins:
                try:
                    if admin_ == request.json['auth']:
                        self.plans[request.json['name']] = request.json['cooldown']
                        self.json_dump_plans()
                        return Response(status=200)

                    else:return Response(status=403)
                except TypeError:
                    return jsonify({'error' : 'invalid_auth', 'status' : 403})

        @app.route('/admin/remove/plan', methods=['POST'])
        @limiter.exempt
        def response_call10():
            for admin_ in self.admins:
                try:
                    if admin_ == request.json['auth']:
                        for plan in self.plans:
                            if plan == request.json['plan']:
                                del plan[request.json['plan']]

                            else:return jsonify({'found' : False, 'status' : 404})

                    else:return Response(status=403)

                except TypeError:
                    return jsonify({'error' : 'invalid auth', 'status' : 403})

    
        self.start_check_stock()
        app.run('0.0.0.0', debug=True)

if __name__ == '__main__':
    API().main()
