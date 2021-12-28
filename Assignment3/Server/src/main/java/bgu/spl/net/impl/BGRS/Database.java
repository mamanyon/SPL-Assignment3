package bgu.spl.net.impl.BGRS;

import bgu.spl.net.impl.BGRS.messages.Acknowledgement;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.DoubleToIntFunction;

/**
 * Passive object representing the Database where all courses and users are stored.
 * <p>
 * This class must be implemented safely as a thread-safe singleton.
 * You must not alter any of the given public methods of this class.
 * <p>
 * You can add private fields and methods to this class as you see fit.
 */
public class Database {
    private Object lock = new Object();
    private HashMap<Integer, Course> courseHashMap; //number course to course

    private ConcurrentHashMap<String, User> userConcurrentHashMap; // username to user

    //to prevent user from creating new Database
    private Database() {
        courseHashMap = new HashMap<>();
        userConcurrentHashMap = new ConcurrentHashMap<>();

        initialize("./Courses.txt");

    }

    private static class SingletonHolder {
        private static Database instance = new Database();
    }

    /**
     * Retrieves the single instance of this class.
     */
    public static Database getInstance() {
        return SingletonHolder.instance;
    }

    /**
     * loades the courses from the file path specified
     * into the Database, returns true if successful.
     */
    boolean initialize(String coursesFilePath) {

        try {
            File courseFile = new File(coursesFilePath);
            Scanner reader = new Scanner(courseFile);
            int courseIndex = 0;

            while (reader.hasNextLine()) {
                String line = reader.nextLine();
                String[] splitLine = line.split("\\|");

                //Initiate course number, name and the number of max student of the course
                int courseNumber = Integer.parseInt(splitLine[0]);
                String courseName = splitLine[1];
                int numOfMaxStudent = Integer.parseInt(splitLine[3]);

                //Initiate Kdam courses vector
                String[] stringKdamCourses = (splitLine[2].substring(1, splitLine[2].length() - 1)).split(",");

                Vector<Integer> kdamCourses = new Vector<>();
                if (!stringKdamCourses[0].equals("")) {
                    for (String courseNum : stringKdamCourses)
                        kdamCourses.add(Integer.parseInt(courseNum));
                }
                //Embed the course to the hash map with the key represented by the course number
                courseHashMap.put(courseNumber, new Course(courseNumber, courseName, kdamCourses, numOfMaxStudent, courseIndex++));
            }
        } catch (FileNotFoundException e) {
            System.out.println("You got a problem with your file path");
            return false;
        }
        return true;
    }

    public boolean addUser(String user, String pass, boolean isAdmin) {
        synchronized (lock) {
            if (userConcurrentHashMap.containsKey(user)) {
                //username already exists
                return false;
            } else {

                if (isAdmin) {
                    User admin = new User(user, pass, true);
                    userConcurrentHashMap.putIfAbsent(user, admin);
                    return true;
                } else //not admin
                {
                    User user1 = new User(user, pass, false);
                    userConcurrentHashMap.putIfAbsent(user, user1);
                    return true;
                }
            }
        }
    }

    public boolean Login(String user, String pass) {

        User login = userConcurrentHashMap.get(user);
        if (login == null || login.isLoggedIn() | !(login.getPassword().equals(pass))) {
            //mo such username || already logged in | password is not correct
            return false;
        } else {
            synchronized (login) {
                login.LogIn();
                return true;
            }
        }
    }

    public boolean Logout(String user) {
        User logout = userConcurrentHashMap.get(user);
        if (logout == null || !(logout.isLoggedIn())) {
            //no such username || not logged in ..
            return false;
        } else {
            synchronized (logout) {
                logout.LogOut();
                return true;
            }
        }
    }

    public boolean CourseRegister(String user, Integer courseNum) {
        User userReg = userConcurrentHashMap.get(user);
        Course toRegister = courseHashMap.get(courseNum);
        if (userReg == null || userReg.getIsAdmin()) return false;
        //user isn't exist or admin can't register to a course
        if (!userReg.isLoggedIn()) return false; //the user is not logged in
        if (toRegister == null) return false; //no such course is exist
        if (isFull(toRegister)) return false; //there's no place in the course.
        if (!hasFinishedKdam(userReg, toRegister)) return false; //the student does not have all the Kdam courses

        synchronized (userReg) {
            synchronized (toRegister) {
                int numOfStudentRegistered = toRegister.getNumRegistered();
                numOfStudentRegistered++;
                toRegister.setNumRegistered(numOfStudentRegistered);
                toRegister.addUser(userReg);
                userReg.getKdamCoursesList().add(courseNum); //register to the course

                return true;
            }
        }
    }

    private boolean hasFinishedKdam(User userReg, Course toRegister) {
        Vector<Integer> userKdamCourses = userReg.getKdamCoursesList();
        for (Integer i : toRegister.getKdamCoursesList()) { //go throw the kdam courses for the specific course
            if (!userKdamCourses.contains(i))
                return false;
        }
        return true;
    }

    private boolean isFull(Course toRegister) {
        return (toRegister.getNumOfMaxStudents() - toRegister.getNumRegistered() == 0);
    }

    //Return true if removed user registration from specific course
    public boolean unregisterToCourse(String username, int courseNumber) {
        User user = userConcurrentHashMap.get(username);
        Course course = courseHashMap.get(courseNumber);
        if (course != null & !user.getIsAdmin()) {

            synchronized (user) {
                synchronized (course) {
                    Integer cast = courseNumber;
                    boolean removedCourse = user.getKdamCoursesList().remove(cast);
                    Vector<User> listOfStudents = course.getListOfStudents();
                    return removedCourse & listOfStudents.remove(user);
                }
            }
        }
        return false;
    }

    //Return user's course list
    public String CheckMyCurrentCourses(String username) {

        User user = userConcurrentHashMap.get(username);
        if (user != null && !user.getIsAdmin()) {
            return user.getKdamCoursesList().toString().replaceAll(" ", "");
        }
        return null;
    }

    //Return Kdam course list of specific course number if exist, otherwise return null
    public String kdamCheck(String userName, int courseNumber) {
        Course course = courseHashMap.get(courseNumber);
        User user = userConcurrentHashMap.get(userName);
        synchronized (course) {

            if (!user.getIsAdmin()) {
                Vector<Integer> KdamCoursesList = course.getKdamCoursesList();
                KdamCoursesList.sort((o1, o2) -> {
                    int courseIndexO1 = courseHashMap.get(o1).getCourseIndex();
                    int courseIndexO2 = courseHashMap.get(o2).getCourseIndex();
                    return courseIndexO1 - courseIndexO2;
                });

                return KdamCoursesList.toString().replaceAll(" ","");
            }
            return null;
        }
    }

    public String CourseStats(String userName, int courseNumber) { // message 7
        String output = null;
        User user = userConcurrentHashMap.get(userName);
        if (user.getIsAdmin()) { // only for admins
            Course course = courseHashMap.get(courseNumber);

            if (course != null) { //course is not exists
                synchronized (course) {
                    output = "Course: " + "(" + course.getCourseNum() + ") " + course.getCourseName() + "\n";
                    int numOfSeatsAvailable = course.getNumOfMaxStudents() - course.getNumRegistered();

                    output += "Seats Available: " + numOfSeatsAvailable + "/" + course.getNumOfMaxStudents() + "\n";
                    Vector<String> usersNameOfStudents = new Vector<>();

                    for (User u : course.getListOfStudents())
                        usersNameOfStudents.add(u.getUsername());
                    Collections.sort(usersNameOfStudents);
                    output += "Students Registered: " + usersNameOfStudents.toString();
                }
            }
        }
        return output;
    }

    public String isRegistered(String username, int courseNumber) { //message 9
        User user = userConcurrentHashMap.get(username);
        synchronized (user) {
            if (courseHashMap.get(courseNumber) != null & !user.getIsAdmin()) {
                Vector<Integer> courseList = user.getKdamCoursesList();
                boolean isRegistered = courseList.contains(courseNumber);
                if (isRegistered) {
                    return "REGISTERED";
                } else {
                    return "NOT REGISTERED";
                }
            } else {
                return "ERR";
            }
        }
    }

    public String StudentsStats(String userName, String studentUsername) { //message 8
        User studentUser = userConcurrentHashMap.get(studentUsername);
        User adminUser = userConcurrentHashMap.get(userName);
        if (studentUser != null) {
            synchronized (studentUser) {
                synchronized (adminUser) {
                    if ((studentUser != null && !studentUser.getIsAdmin()) & (adminUser != null && adminUser.getIsAdmin())) { //  only for admins
                        String output = "Student:" + studentUser.getUsername() + "\n";
                        Vector<Integer> courses = studentUser.getKdamCoursesList();
                        courses.sort((o1, o2) -> {
                            int courseIndexO1 = courseHashMap.get(o1).getCourseIndex();
                            int courseIndexO2 = courseHashMap.get(o2).getCourseIndex();
                            return courseIndexO1 - courseIndexO2;
                        });
                        output += "Courses:" + courses.toString().replaceAll(" ", "");
                        return output;
                    }
                }
            }
        }
        return null;
    }
}

